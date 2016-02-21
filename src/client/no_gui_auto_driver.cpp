#include "no_gui_auto_driver.hpp"
#include "client_game_objects_factory.hpp"
#include "network_maze_loader.hpp"

no_gui_auto_driver::no_gui_auto_driver(int players_number_)
    : players_number(players_number_)
{
}

int no_gui_auto_driver::run(const std::string &ip_address)
{
    assert(players_number <= 1500);
    std::vector<std::shared_ptr<networking::client>> clients(players_number);
    std::vector<std::shared_ptr<networking::network_manager>> network_managers(players_number);
    std::vector<std::shared_ptr<core::client_game_objects_factory>> factories(players_number);

    for (int i = 0; i < players_number; i++)
    {
        clients[i] = std::make_shared<networking::client>(ip_address);;
        network_managers[i] = std::make_shared<networking::network_manager>(clients[i]);
        factories[i] = std::make_shared<core::client_game_objects_factory>(nullptr,
                                                                           nullptr,
                                                                           network_managers[i]);
        world_managers.push_back(std::make_shared<core::client_world_manager>(factories[i],
                                                                              network_managers[i],
                                                                              true));

        world_managers.back()->make_maze(std::make_shared<networking::network_maze_loader>(clients[i]));
        world_managers.back()->load_all();
    }

    try
    {
        timer.async_wait([this](auto error_code){ this->tick(error_code); });
        m_io_service.run();
    }
    catch (std::exception& exception)
    {
        logger_.log("exception: %s", exception.what());
    }
    // TO DO: Doesn't work yet
    for (int i = 0; i < players_number; i++)
    {
        world_managers[i]->shut_down_client();
    }
    return 0;
}

void no_gui_auto_driver::tick(const boost::system::error_code&)
{
    static size_t manager_id = 0;

    if (world_managers[manager_id] != nullptr)
        world_managers[manager_id]->tick_all();

    manager_id = (manager_id+1)%world_managers.size();

    timer.expires_at(timer.expires_at() + interval);
    timer.async_wait([this](auto error_code){ this->tick(error_code); });
}
