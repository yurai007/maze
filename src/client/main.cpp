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

 * there was (buggy??) make_shared in run for auto client =  std::make_shared<networking::client>(ip_address);

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

 * now group_driver works as expected (despite verification problem on server). Static-s on client side
   was the reason.

 * TODO:
   - signals in cmd_driver doesn't work -> shut_down doesn't work (but for gui-on everything is OK)
   - now I need more enemies (~50). Generate them randomly.
   - now in order to run e.g 50 players I need 50 separated directories. But I would like to
     generate ~5000 players. I need some event-driven generator like for 1024k problem.
*/

class gui_driver
{
public:
    gui_driver(int argc_, char** argv_)
        : argc(argc_),
          argv(argv_)
    {
    }

    int run(const std::string &ip_address)
    {
        application = Gtk::Application::create(argc, argv, "");

        auto qt_controller = std::make_shared<control::controller>();
        auto qt_renderer = std::make_shared<presentation::renderer>();
//        auto client =  std::make_shared<networking::client>(ip_address);
        std::shared_ptr<networking::client> client(new networking::client(ip_address));
        auto game_objects_factory = std::make_shared<core::client_game_objects_factory>(qt_renderer,
                                                                                        qt_controller,
                                                                                        client);
        auto world_manager = std::make_shared<core::client_world_manager>(game_objects_factory,
                                                                          client,
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

private:
    Glib::RefPtr<Gtk::Application> application;
    int argc;
    char **argv;
};


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

//    void stop()
//    {
//        m_io_service.stop();
//        world_manager->shut_down_client();
//    }

    int run(const std::string &ip_address)
    {
        auto client =  std::make_shared<networking::client>(ip_address);
        auto game_objects_factory = std::make_shared<core::client_game_objects_factory>(nullptr,
                                                                                        nullptr,
                                                                                        client);

        world_manager = std::make_shared<core::client_world_manager>(game_objects_factory,
                                                                          client,
                                                                          true);

        world_manager->make_maze(std::make_shared<networking::network_maze_loader>(client));
        world_manager->load_all();

//        boost::asio::signal_set m_signals(m_io_service);
//        m_signals.add(SIGINT);
//        m_signals.add(SIGTERM);
//      #if defined(SIGQUIT)
//        m_signals.add(SIGQUIT);
//      #endif
//         m_signals.async_wait(boost::bind(&boost::asio::io_service::stop, &m_io_service));

        try
        {
            timer.async_wait(boost::bind(&cmd_driver::tick, this, placeholders::error));
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

private:
    int argc;
    char **argv;

    io_service m_io_service;
    boost::posix_time::milliseconds interval {30};
    deadline_timer timer {m_io_service, interval};


    std::shared_ptr<core::client_world_manager> world_manager {nullptr};
};


class group_driver
{
public:
    group_driver(int argc_, char** argv_, int players_number_)
        : argc(argc_),
          argv(argv_),
          players_number(players_number_)
    {
    }

    void tick(const boost::system::error_code&)
    {
        for (int i = 0; i < players_number; i++)
        {
            if (world_managers[i] != nullptr)
                world_managers[i]->tick_all();
            usleep(1*1000); //1ms
        }

        timer.expires_at(timer.expires_at() + interval);
        timer.async_wait(boost::bind(&group_driver::tick, this, placeholders::error));
    }

    int run(const std::string &ip_address)
    {
        assert(players_number <= 128);
        std::vector<std::shared_ptr<networking::client>> clients(players_number);
        std::vector<std::shared_ptr<core::client_game_objects_factory>> factories(players_number);

        for (int i = 0; i < players_number; i++)
        {
            clients[i] = std::make_shared<networking::client>(ip_address);
            factories[i] = std::make_shared<core::client_game_objects_factory>(nullptr,
                                                                               nullptr,
                                                                               clients[i]);
            world_managers.push_back(std::make_shared<core::client_world_manager>(factories[i],
                                                                                  clients[i],
                                                                                  true));

            world_managers.back()->make_maze(std::make_shared<networking::network_maze_loader>(clients[i]));
            world_managers.back()->load_all();
        }

        try
        {
            timer.async_wait(boost::bind(&group_driver::tick, this, placeholders::error));
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

private:
    int argc;
    char **argv;

    io_service m_io_service;
    boost::posix_time::milliseconds interval {30};
    deadline_timer timer {m_io_service, interval};

    int players_number;
    std::vector<std::shared_ptr<core::client_world_manager>> world_managers;
};

void generator_test_case()
{
    utils::maze_generator generator(50);
    generator.generate_with_patterns_from("patterns.in", 3);
    generator.save_to_file("maze.txt");
}

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        logger_.log("Usage: ./maze_client [ip_address] [mode]");
        return 1;
    }

    logger_.log("Arguments: ip_address = %s, mode = %s", argv[1], argv[2]);
    const std::string ip_address(argv[1]), mode(argv[2]);

    if (mode == "gui-on")
    {
        gui_driver driver(0, NULL);
        return driver.run(ip_address);
    }
    else
    if (mode == "gui-off")
    {
        cmd_driver driver(0, NULL);
        return driver.run(ip_address);
    }
    else
        if (mode == "many")
        {
            group_driver driver(0, NULL, 40);
            return driver.run(ip_address);
        }
    else
    {
        logger_.log("Bad mode");
        return 1;
    }
}

