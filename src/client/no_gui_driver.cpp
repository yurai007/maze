#include "no_gui_driver.hpp"
#include "client_game_objects_factory.hpp"
#include "network_maze_loader.hpp"

#include "renderer.hpp"
#include "controller.hpp"

int no_gui_driver::run(const std::string &ip_address)
{
    auto client = smart::smart_make_shared<networking::client>(ip_address);
    auto network_manager =  smart::smart_make_shared<networking::network_manager>(client);

    // workaraound
    smart::fit_smart_ptr<presentation::renderer> dummy_renderer(nullptr);
    smart::fit_smart_ptr<control::controller>  dummy_controller(nullptr);

    auto game_objects_factory = smart::smart_make_shared<core::client_game_objects_factory>(dummy_renderer,
                                                                                    dummy_controller,
                                                                                    network_manager);

    world_manager = std::make_shared<core::client_world_manager>(game_objects_factory,
                                                                      network_manager,
                                                                      true);

    world_manager->make_maze(smart::smart_make_shared<networking::network_maze_loader>(client));
    world_manager->load_all();

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
    world_manager->shut_down_client();
    return 0;
}

void no_gui_driver::tick(const boost::system::error_code&)
{
    if (world_manager != nullptr)
          world_manager->tick_all();

    timer.expires_at(timer.expires_at() + interval);
    timer.async_wait([this](auto error_code){ this->tick(error_code); });
}
