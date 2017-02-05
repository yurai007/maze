#include "../common/maze_generator.hpp"
#include "async_logger.hpp"

#include "no_gui_auto_driver.hpp"
#include "no_gui_driver.hpp"
#include "gui_driver.hpp"

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

 * there was (buggy??) make_shared in run for auto client =  smart::smart_make_shared<networking::client>(ip_address);

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

 * for some reason synchronous client doesn't support in boost asio signal handling
   (boost::asio::signal_set doesn't work). Client destructor isn't called too (WTF?, strange).

 * Instead asynchronous signals calling I decided to use plain synchronous
   call: world_manager->shut_down_client(); after end of gtk event loop.

 * WTF? Destructors aren't called? Problems with shared_ptr destructors.
   - I can ignore that and call make_player only if in position_to_player_id but it means I must someway
     defer make_player call - game_objects will be added during ticking.

 * bool_to_string is quite tricky and comes from:
   ref: http://stackoverflow.com/questions/7617479/best-way-to-print-the-result-of-a-bool-as-false-or-true-in-c

 * why this fucking resolver doesn't work for loopback? Another constructor with
   tcp::resolver::query::canonical_name is needed

 * now no_gui_auto_driver works as expected (despite verification problem on server). Static-s on client side
   was the reason.

 * For more than 254 players boost asio on client side throws exception - eventfd_select_interruptter:
   Too may open files.

 * modyfing /etc/security/limits.conf solves problem with too many open files.

 * now for 512 players I have performance problem with maze_client.
   Maze_client's timer (release build) can't perform ticking with resolution = 2ms. In logs I see
   ticking every ~4-5ms. The bottleneck is probably logging itself which slow down whole process.
   I must check this but reducing logging (only to started tick/finish tick) and check timings
   Possible solutions:
   - limit whole logging. Now I get after few minutes ~1GB logs. I can do this easly but it's boring.
   - limit flushing. I guess flushing is the performance killer here.
   - asynchronous logger. Logging would be on different (slow) I/O thread. Comunication between threads
     by queue. Probably the best sollution.

   I don't see any problems with maze_server now (even for debug build) but notice there is no network
   communication (according to gnome monitor). Maze_server seems to be CPU-bound but as I wrote
   1ms ticking for server is easy (for 512 clients and loopback :)

 * after limiting logging to only few lines per client_world_manager and disabling flushing in logger
   overhead per tick decreased to ~2ms. I guess with asynchronous logger I achive ~1ms or less.

 * never ever call malloc for c++ classes. Strange things will happen like invalid pointer error because
   malloc doesn't call constructors.
   In C++ always new/placement new + delete/destructor call.

 * According to stats_512.txt overeall latency is quite well (But notice I don't log too much).
   Average latency ~ 1.2ms and only ~15% is >=2 ms.
   There are sometimes big slow downs 1x 410ms and 1x 535ms. Couple of ~10ms-15ms.
   I should prepare some fake logger test which will be contain only logging and produce log.txt.

 * Finally new workflow with scripts. Just:
   1. cdmaze
   2. cd scripts
   3. source run.sh release 512
   4. source cleanup.sh

   For latency statistics from logs:
   1. cd bin/release
   2. source ../latency_stat.sh

 * everything is fine on stress test for 1500 players.

 * TODO:
   - logger test. In pararell dynamic view for ability to run ~2k players:) Then server ticking
     ~300us.
   - speed up maze_client for 512 players by reducing ticking from 4-5ms to 2ms
   - signals in no_gui_driver doesn't work -> shut_down doesn't work (but for gui-on everything is OK)

 * TO DO fresh:

   - on server side - vector resources and map_* methods should be probably removed because
     full information about resource is encoded in maze. Then whole part of resources handling in
     tick_all should be removed.
*/

void generator_test_case()
{
    utils::maze_generator generator(50);
    generator.generate_with_patterns_from("patterns.in", 3);
    generator.save_to_file("maze.txt");
}

int main(int argc, char** argv)
{
    logger_.run();
    if (argc < 3)
    {
        logger_.log("Usage: ./maze_client [ip_address] [mode] [players_number]");
        return 1;
    }

    if (argc == 4)
        logger_.log("Arguments: ip_address = %s, mode = %s, players_number = %s",
                    argv[1], argv[2], argv[3]);
    else
        logger_.log("Arguments: ip_address = %s, mode = %s",
                    argv[1], argv[2]);
    const std::string ip_address(argv[1]), mode(argv[2]);

    if (mode == "gui-on")
    {
        gui_driver driver(0, NULL);
        return driver.run(ip_address);
    }
    else
    if (mode == "gui-off")
    {
        no_gui_driver driver;
        return driver.run(ip_address);
    }
    else
        if (mode == "many")
        {
            assert(argc == 4);
            int players_number = std::stoi(std::string(argv[3]));
            no_gui_auto_driver driver(players_number);
            return driver.run(ip_address);
        }
    else
    {
        logger_.log("Bad mode");
        return 1;
    }
}

