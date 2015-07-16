#include "maze_generator.hpp"
#include "logger.hpp"
//#include "creature.hpp"
//#include "enemy.hpp"
//#include "maze.hpp"
#include "world_manager.hpp"
#include "renderer.hpp"
#include "controller.hpp"

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
        auto world_manager = std::make_shared<core::world_manager>(qt_renderer, qt_controller);

        world_manager->add_maze();
        world_manager->add_player();
        world_manager->load_all();
        qt_renderer->set_world(world_manager);


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

void generator_test_case()
{
    utils::maze_generator generator(50);
    generator.generate_with_patterns_from("patterns.in", 3);
    generator.save_to_file("maze.txt");
}

int main(int argc, char** argv)
{
    gui_driver qt_driver(argc, argv);
    return qt_driver.run();
}

