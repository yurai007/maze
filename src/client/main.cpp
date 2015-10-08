#include <thread>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "../common/maze_generator.hpp"
#include "../common/logger.hpp"
#include "../common/controller.hpp"
#include "../common/message_dispatcher.hpp"
#include "../common/network_maze_loader.hpp"
#include "client_game_objects_factory.hpp"
#include "client_world_manager.hpp"
#include "renderer.hpp"
#include "client.hpp"

using namespace boost::asio;

/*
 * Great makefile tutorial:
   https://sites.google.com/site/michaelsafyan/software-engineering/how-to-write-a-makefile

 * for run(): Gtk::Window window; and presentation::renderer qt_renderer; can't be member classes!
   Moving from run cause some awful Gtk assertions failing.

 * Some random error related with free() after application closing:)

 * Some random "http://stackoverflow.com/questions/1345670/stack-smashing-detected"
   during app closing. It turned out that making qt_renderer and window 'static' solved problem.
   Probably after adding extra 'renderer*' field to maze SO randomly occured.

 * Suppose renderer has : public enable_shared_from_this. Despite that I cannot use shared_from_this
   in renderer constructor because I get std::bad_weak_ptr exception. So I decided to introduce setter
   in renderer.

 * 1. rvalue reference: int &&x = 123; Foo &&foo = Foo();
   2. move-semantics: emplace_back(123) but emplace_back(std::move(foo)) where foo is left value.
   3. world_manager is unique_ptr
   
 * valgrind "invalid read of size" helped me a lot in finding serious problem with maze.txt
 * still many "possibly lost" reports!

 * local objects are cool but we must remember about lifetime. Local objects are dangerous in
   callbacks/handlers.

 * WTF?? Emplace_back problem in world_manager::add_enemy and enemy::id.
   I had to replace emplace_back to push_back.

 * I must cheat gtkmm/gui_driver that I haven't any arguments. Otherwise error from gtkmm:
    GLib-GIO-CRITICAL **: This application can not open files.

 * cyclic reference are paintful and forward declarations are needed to break it

 * unfortunately gtkmm is fucking piece of shit and there are so huge lags only for 3 clients on
   one host that running multiple clients with GUI on one machine doesn't make sense.
   So the roadmap is:
   1. Disable gui and add to client_players enemy_server's logic for movement
   2. Try this on one machine with up to 5 clients (one with GUI)
   3. Add huge enemies number for introducing network congestion
   4. Repeat for 2 machines.
*/

class gui_driver
{
public:
    gui_driver(int argc_, char** argv_)
        : argc(argc_),
          argv(argv_)
    {
    }

    int run()
    {
        application = Gtk::Application::create(argc, argv, "");

        auto qt_controller = std::make_shared<control::controller>();
        auto qt_renderer = std::make_shared<presentation::renderer>();

        //auto client =  std::make_shared<networking::client>(); // WTF?? Buggy make_shared???
        std::shared_ptr<networking::client> client(new networking::client());
        std::shared_ptr<core::client_game_objects_factory> game_objects_factory(
                                new core::client_game_objects_factory(qt_renderer,
                                qt_controller,
                                client));

        auto world_manager = std::make_shared<core::client_world_manager>(game_objects_factory,
                                                                          client);

        world_manager->make_maze(std::make_shared<networking::network_maze_loader>(client));
        world_manager->load_all();
        qt_renderer->set_world(world_manager);


        qt_controller->set_title("The Maze");
        qt_controller->set_default_size(1024, 768);

        qt_controller->add(*qt_renderer);
        qt_renderer->show();

        int result = application->run(*qt_controller);
        return result;
    }

private:
    Glib::RefPtr<Gtk::Application> application;
    int argc;
    char **argv;
};

void generator_test_case()
{
    utils::maze_generator generator(50);
    generator.generate_with_patterns_from("patterns.in", 3);
    generator.save_to_file("maze.txt");
}

int main(int argc, char** argv)
{
    if (argc > 1)
        logger_.log("Arg: %s", argv[1]);

    gui_driver driver(0, NULL);
    return driver.run();
}

