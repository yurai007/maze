#include "maze_generator.hpp"
#include "logger.hpp"
#include "world_manager.hpp"
#include "renderer.hpp"
#include "controller.hpp"
#include "message_dispatcher.hpp"

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
        auto world_manager = std::make_unique<core::world_manager>(qt_renderer, qt_controller);

        world_manager->add_maze();
        world_manager->load_all();
        qt_renderer->set_world(std::move(world_manager));


        qt_controller->set_title("The Maze");
        qt_controller->set_default_size(1024, 768);

        qt_controller->add(*qt_renderer);
        qt_renderer->show();

        return application->run(*qt_controller);
    }

private:
    Glib::RefPtr<Gtk::Application> application;
    int argc;
    char **argv;
};

void message_dispatcher_test_case()
{
    int foo = 0;
    networking::message_dispatcher dispatcher;

    dispatcher.add_handler( [&] (std::string const& msg1)
    {
        logger_.log("Got a %s and %d", msg1.c_str(), foo);
    });

    dispatcher.add_handler( [] (int msg2)
    {
       logger_.log("Got a %d", msg2);
    });

    dispatcher.add_handler( [] (double msg3)
    {
        logger_.log("Got a %f", msg3);
    });

    dispatcher.dispatch(dispatcher);
    dispatcher.dispatch(42);
    dispatcher.dispatch("pupka");
    dispatcher.dispatch(42.0123);
    foo = 666;
    dispatcher.dispatch(std::string("dupa"));

}

void generator_test_case()
{
    utils::maze_generator generator(50);
    generator.generate_with_patterns_from("patterns.in", 3);
    generator.save_to_file("maze.txt");
}

int main(int argc, char** argv)
{
    message_dispatcher_test_case();
    gui_driver qt_driver(argc, argv);
    return qt_driver.run();
}

