#ifndef CLIENT_GAME_OBJECTS_FACTORY_HPP
#define CLIENT_GAME_OBJECTS_FACTORY_HPP

#include <memory>
#include "../common/maze_loader.hpp"
#include "../client/client_player.hpp"
#include "../client/client_resource.hpp"
#include "../client/client_enemy.hpp"
#include "../client/client_maze.hpp"

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

class client_player;
class client_world_manager;

class client_game_objects_factory
{
public:
    client_game_objects_factory(std::shared_ptr<presentation::renderer> renderer,
                         std::shared_ptr<control::controller> controller,
                         std::shared_ptr<networking::client> client);

    std::shared_ptr<client_maze> create_client_maze(std::shared_ptr<maze_loader> loader);
    std::shared_ptr<client_player> create_client_player(int id, int posx, int posy, bool active);
    std::shared_ptr<client_enemy> create_client_enemy(
            std::shared_ptr<core::client_world_manager> manager,
            int posx, int posy, int id);
    std::shared_ptr<client_resource> create_client_resource(const std::string &name, int posx, int posy);
private:
    std::shared_ptr<presentation::renderer> renderer_;
    std::shared_ptr<control::controller> controller_;
    std::shared_ptr<networking::client> client_;
    std::shared_ptr<core::client_maze> maze_;
};

}

#endif // CLIENT_GAME_OBJECTS_FACTORY_HPP

