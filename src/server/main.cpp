#include "../common/maze_generator.hpp"
#include "../common/logger.hpp"
#include "../common/world_manager.hpp"
#include "../common/renderer.hpp"
#include "../common/controller.hpp"
#include "../common/message_dispatcher.hpp"
#include "game_server.hpp"
#include "../common/file_maze_loader.hpp"

#include <thread>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

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

 TO DO:
    Disable gui in server

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
        auto world_manager = std::make_shared<core::world_manager>(qt_renderer, qt_controller, nullptr);

        world_manager->add_maze(std::make_shared<core::file_maze_loader>());
        world_manager->load_all();
        qt_renderer->set_world(world_manager);


        qt_controller->set_title("The Maze");
        qt_controller->set_default_size(1024, 768);

        qt_controller->add(*qt_renderer);
        qt_renderer->show();

        networking::game_server server;

        // thread 2
        std::thread network_thread([&]()
        {
            try
            {
                // I have no fucking idea why removing emplaces,
                // std::move (with unique_ptr) and moving world_manager->maze_ here
                // fixed lock_guard...
                server.init(world_manager->maze_);
                server.run();
            }
            catch (std::exception& e)
            {
                logger_.log("exception: %s", e.what());
            }

        });
        int result = application->run(*qt_controller);
        server.stop(); // without that network_thread is still working and
        // cannot be join so join blocks and application hangs.
        network_thread.join();

        return result;
    }

private:
    Glib::RefPtr<Gtk::Application> application;
    int argc;
    char **argv;
};

// Only one thread
class cmd_driver
{
public:
    cmd_driver(int argc_, char** argv_)
        : argc(argc_),
          argv(argv_)
    {
    }

    void tick(const boost::system::error_code&)
    {
        if (world_manager != nullptr)
              world_manager->tick_all();

        timer.expires_at(timer.expires_at() + interval);
        timer.async_wait(boost::bind(&cmd_driver::tick, this, placeholders::error));
    }

    int run()
    {
        world_manager->add_maze(std::make_shared<core::file_maze_loader>());
        world_manager->load_all();

        try
        {
            timer.async_wait(boost::bind(&cmd_driver::tick, this, placeholders::error));
            server.init(world_manager->maze_);
            server.run();
        }
        catch (std::exception& e)
        {
            logger_.log("exception: %s", e.what());
        }
        return 0;
    }

private:
    int argc;
    char **argv;

    networking::game_server server;
    boost::posix_time::milliseconds interval {30};
    deadline_timer timer {server.main_server.m_io_service, interval};
    std::shared_ptr<core::world_manager> world_manager
            {std::make_shared<core::world_manager>(nullptr, nullptr, nullptr)};
};

using namespace networking::messages;

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

    cmd_driver driver(0, NULL);
    return driver.run();
}

