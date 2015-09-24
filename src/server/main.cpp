#include <thread>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "../common/maze_generator.hpp"
#include "../common/logger.hpp"
#include "../common/message_dispatcher.hpp"
#include "../common/file_maze_loader.hpp"
#include "server_world_manager.hpp"
#include "game_server.hpp"

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
              world_manager->tick_all(false);

        timer.expires_at(timer.expires_at() + interval);
        timer.async_wait(boost::bind(&cmd_driver::tick, this, placeholders::error));
    }

    int run()
    {
        world_manager->make_maze(std::make_shared<core::file_maze_loader>());
        world_manager->load_all();

        try
        {
            timer.async_wait(boost::bind(&cmd_driver::tick, this, placeholders::error));
            server.init(world_manager->get_maze(), world_manager);
            server.run();
        }
        catch (std::exception& exception)
        {
            logger_.log("exception: %s", exception.what());
        }
        return 0;
    }

private:
    int argc;
    char **argv;

    networking::game_server server;
    boost::posix_time::milliseconds interval {30};
    deadline_timer timer {server.main_server.m_io_service, interval};

    std::shared_ptr<core::server_game_objects_factory> game_objects_factory
        {std::make_shared<core::server_game_objects_factory>(nullptr)};

    std::shared_ptr<core::server_world_manager> world_manager
        {std::make_shared<core::server_world_manager>(game_objects_factory)};

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

