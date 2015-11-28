#include <boost/bind.hpp>

#include "no_gui_driver.hpp"
#include "client_game_objects_factory.hpp"
#include "network_maze_loader.hpp"

int no_gui_driver::run(const std::string &ip_address)
{
    auto client =  std::make_shared<networking::client>(ip_address);
    auto network_manager =  std::make_shared<networking::network_manager>(client);
    auto game_objects_factory = std::make_shared<core::client_game_objects_factory>(nullptr,
                                                                                    nullptr,
                                                                                    network_manager);

    world_manager = std::make_shared<core::client_world_manager>(game_objects_factory,
                                                                      network_manager,
                                                                      true);

    world_manager->make_maze(std::make_shared<networking::network_maze_loader>(client));
    world_manager->load_all();

    try
    {
        timer.async_wait(boost::bind(&no_gui_driver::tick, this, placeholders::error));
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
    timer.async_wait(boost::bind(&no_gui_driver::tick, this, placeholders::error));
}
