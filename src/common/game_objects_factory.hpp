#ifndef GAME_OBJECTS_FACTORY_HPP
#define GAME_OBJECTS_FACTORY_HPP

#include <memory>
#include "maze_loader.hpp"
#include "maze.hpp"
#include "remote_player.hpp"
#include "enemy.hpp"
#include "resource.hpp"

namespace presentation
{
    class renderer;
}

namespace control
{
    class controller;
}

namespace networking
{
    class client;
}

namespace core
{

class game_objects_factory
{
public:
    game_objects_factory(std::shared_ptr<presentation::renderer> renderer,
                         std::shared_ptr<control::controller> controller,
                         std::shared_ptr<networking::client> client);
    std::shared_ptr<maze> create_maze(std::shared_ptr<maze_loader> loader);
    std::shared_ptr<remote_player> create_remote_player(int posx, int posy);
    std::shared_ptr<enemy> create_enemy(int posx, int posy);
    std::shared_ptr<resource> create_resource(const std::string &name, int posx, int posy);
private:
    std::shared_ptr<presentation::renderer> renderer_;
    std::shared_ptr<control::controller> controller_;
    std::shared_ptr<networking::client> client_;
    std::shared_ptr<core::maze> maze_;
};

}

#endif // GAME_OBJECTS_FACTORY_HPP
