#include <thread>
#include <boost/asio.hpp>

#include "../common/maze_generator.hpp"
#include "../common/logger.hpp"
#include "../common/message_dispatcher.hpp"
#include "../common/file_maze_loader.hpp"
#include "../common/smart_ptr.hpp"

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
   in renderer constructor because I get std::bad_weak_ptr exception. So I decided to introduce
   setter in renderer.

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
    Add get_players_data msg to client and server. Then I will be allowed to run many clients on
    localhost!

 TO DO:
    only when server and client will be clean uder valgrind I can replace std::shared_ptr by
    fit_smart_ptr

    87 errors

    1. Invalid reads in server::handle_stop.
    2. Many allocations and leaks from gtk. Use case only with server
       without manager.

     After removing this fucking gtk dependency (ldd + `pkg-config gtkmm-3.0 --cflags --libs`):

     total heap usage: 2,097 allocs, 1,859 frees, 173,714 bytes allocated
     decreased to
     total heap usage: 33 allocs, 33 frees, 25,860 bytes allocated
     and no more leaks!

   3. Many cyclic dependency:
      server_game_objects_factory -> server_world_manager -> server_game_objects_factory
      server_world_manager -> server_player -> server_world_manager.
      FIX: There was only dependecy to one map so I moved this on caller side

   4. Problem with resource cleanup in server_world_manager::tick_all.

   TO DO: I need polymorphism (for upcasting), dynamic_pointer_cast and enable_shared_from_this.
          Without this I can't use my smart_ptr.hpp

*/

class server_driver
{
public:
    server_driver()
    {
        assert(world_manager != nullptr);
    }

    int run()
    {
        world_manager->make_maze(smart::smart_make_shared<core::file_maze_loader>());
        world_manager->load_all();

        try
        {
            timer.async_wait([this](auto error_code){ this->tick(error_code); });
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

    void tick(const boost::system::error_code&)
    {
        world_manager->tick_all();
        timer.expires_at(timer.expires_at() + interval);
        timer.async_wait([this](auto error_code){ this->tick(error_code); });
    }

    networking::game_server server;
    boost::posix_time::milliseconds interval {1};
    deadline_timer timer {server.get_io_service(), interval};

    smart::fit_smart_ptr<core::server_game_objects_factory> game_objects_factory
        {smart::smart_make_shared<core::server_game_objects_factory>()};

    smart::fit_smart_ptr<core::server_world_manager> world_manager
        {smart::smart_make_shared<core::server_world_manager>(game_objects_factory)};

};

using namespace networking::messages;

int main(int argc, char** argv)
{
    if (argc > 1)
        logger_.log("Arg: %s", argv[1]);

    server_driver driver;
    driver.run();
    return 0;
}

