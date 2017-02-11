#include <thread>

#include "../common/maze_generator.hpp"
#include "../common/logger.hpp"
#include "../common/message_dispatcher.hpp"
#include "../common/file_maze_loader.hpp"
#include "../common/smart_ptr.hpp"

#include "server_world_manager.hpp"
#include "game_server.hpp"

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

   REPLACING SHARED_PTR BY FIT_SMART_PTR CONT:

   Server plan:

   Done.

   Client plan:

   0. Shared_from_this and dynamic_pointer_cast are not supported by my fir_smart_ptr.
   1. Grouping object by types in vectors or sth like that to avoid checking and downcasting
      by dynamic_pointer_cast?
   2. Shared_from_this is only needed if we wrap passing object in shared_ptr.
      Shared_from_this() may be removed from make_player and make_enemy. Lifetime of world_manager
      is longer then players and enemies lifetime so in client_player/client_enemy:

      shared_ptr

      std::shared_ptr<core::client_world_manager> manager;

      may be replaced by reference

      core::client_world_manager &manager;

   Common plan:

   1. std::dynamic_pointer_cast<client_maze>(maze); so because of lack of
      dynamic_pointer_cast abstract_maze must use this fucking shared_ptr but...
   2. Some mess with abstract_maze on client side. Replace abstract_maze by client_maze and
      downcasting may be removed

   Added extra protectors and ASan:
        CXXFALGS += -fsanitize-recover=address
        ASAN_OPTIONS=halt_on_error=0 ./maze_server


   * Be careful with coversions between signed and unsigned types. Look at bug in
     abstract_maze::get_extended_field (overflow for signed char < 0 to unsigned short)


   TO DO1: now resources are blown up by 'fireball' - should be removed on server side
   TO DO2: add fireballs on client side
   TO DO3: move fireball drawing from client_maze::draw to client_world_manager::draw_all
   TO DO4: automatic players should throw fireballs as well
   TO DO5: fireball should disappear after some time (~10s ?)
   TO DO6: boss?

*/

class server_driver
{
public:
    server_driver(bool pause_for_test)
        : pause_mode(pause_for_test)
    {
        assert(world_manager != nullptr);
    }

    int run()
    {
        world_manager->load_all(smart::smart_make_shared<core::file_maze_loader>());

        try
        {
            if (!pause_mode)
                server.get_reactor().add_timer_handler(1, [=](){
                    world_manager->tick_all();
                });
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

    bool pause_mode;
    networking::game_server server;

    smart::fit_smart_ptr<core::server_game_objects_factory> game_objects_factory
        {smart::smart_make_shared<core::server_game_objects_factory>()};

    smart::fit_smart_ptr<core::server_world_manager> world_manager
        {smart::smart_make_shared<core::server_world_manager>(game_objects_factory)};
};

using namespace networking::messages;

int main(int argc, char** argv)
{
    if (argc > 1)
    {
        logger_.log("Arguments: mode = %s", argv[1]);
        const std::string mode(argv[1]);
        if (mode != "pause-for-test")
        {
            logger_.log("Usage: ./maze_server [mode]");
            return 0;
        }
        server_driver driver(true);
        driver.run();
    }
    server_driver driver(false);
    driver.run();
    return 0;
}

