#include "no_gui_auto_driver.hpp"
#include "client_game_objects_factory.hpp"
#include "network_maze_loader.hpp"

#include "renderer.hpp"
#include "controller.hpp"

no_gui_auto_driver::no_gui_auto_driver(int players_number_)
    : players_number(players_number_)
{
}

int no_gui_auto_driver::run(const std::string &ip_address)
{
    assert(players_number <= 1500);
    std::vector<smart::fit_smart_ptr<networking::client>> clients(players_number);
    std::vector<smart::fit_smart_ptr<networking::network_manager>> network_managers(players_number);
    std::vector<smart::fit_smart_ptr<core::client_game_objects_factory>> factories(players_number);

    for (int i = 0; i < players_number; i++)
    {
        clients[i] = smart::smart_make_shared<networking::client>(ip_address);;
        network_managers[i] = smart::smart_make_shared<networking::network_manager>(clients[i]);

        // workaraound
        smart::fit_smart_ptr<presentation::renderer> dummy_renderer(nullptr);
        smart::fit_smart_ptr<control::controller>  dummy_controller(nullptr);

        factories[i] = smart::smart_make_shared<core::client_game_objects_factory>(dummy_renderer,
                                                                           dummy_controller,
                                                                           network_managers[i]);
        world_managers.push_back(smart::smart_make_shared<core::client_world_manager>(factories[i],
                                                                              network_managers[i],
                                                                              true));

        world_managers.back()->make_maze(smart::smart_make_shared<networking::network_maze_loader>(
                                             clients[i]));
        world_managers.back()->load_all();
    }

    try
    {
        int ms = std::max(2, 1000/players_number);
        logger_.log("ms: %d", ms);
        interval = std::make_unique<boost::posix_time::milliseconds>(ms);
        timer = std::make_unique<deadline_timer>(m_io_service, *interval);
        timer->async_wait([this](auto error_code){ this->tick(error_code); });

        m_signals.add(SIGINT);
        m_signals.add(SIGTERM);
        m_signals.add(SIGQUIT);
        m_signals.async_wait([this](auto, auto){
            this->stop_all();
        });

        m_io_service.run();
    }
    catch (std::exception& exception)
    {
        logger_.log("exception: %s", exception.what());
    }

    return 0;
}

void no_gui_auto_driver::tick(const boost::system::error_code&)
{
    static size_t manager_id = 0;

    if (world_managers[manager_id] != nullptr)
        world_managers[manager_id]->tick_all();

    manager_id = (manager_id+1)%world_managers.size();

    timer->expires_at(timer->expires_at() + *interval);
    timer->async_wait([this](auto error_code){ this->tick(error_code); });
}

void no_gui_auto_driver::stop_all()
{
    for (int i = 0; i < players_number; i++)
    {
        if (!world_managers[i]->killed)
            world_managers[i]->shut_down_client();
    }
}
