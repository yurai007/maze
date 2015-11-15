#include "gui_driver.hpp"
#include "client_game_objects_factory.hpp"
#include "../common/network_maze_loader.hpp"
#include "../common/controller.hpp"

gui_driver::gui_driver(int argc, char **argv)
    : argc_(argc), argv_(argv)
{
}

int gui_driver::run(const std::string &ip_address)
{
    application = Gtk::Application::create(argc_, argv_, "");

    auto qt_controller = std::make_shared<control::controller>();
    auto qt_renderer = std::make_shared<presentation::renderer>();
//        auto client =  std::make_shared<networking::client>(ip_address);
    std::shared_ptr<networking::client> client(new networking::client(ip_address));
    std::shared_ptr<networking::network_manager> network_manager(
                new networking::network_manager(client));
    auto game_objects_factory = std::make_shared<core::client_game_objects_factory>(qt_renderer,
                                                                                    qt_controller,
                                                                                    network_manager);
    auto world_manager = std::make_shared<core::client_world_manager>(game_objects_factory,
                                                                      network_manager,
                                                                      false);

    world_manager->make_maze(std::make_shared<networking::network_maze_loader>(client));
    world_manager->load_all();
    qt_renderer->set_world(world_manager);

    qt_controller->set_title("The Maze");
    qt_controller->set_default_size(1024, 768);
    qt_controller->add(*qt_renderer);

    qt_renderer->show();

    int result = application->run(*qt_controller);
    world_manager->shut_down_client();

    return result;
}
