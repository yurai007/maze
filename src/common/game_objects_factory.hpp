#ifndef GAME_OBJECTS_FACTORY_HPP
#define GAME_OBJECTS_FACTORY_HPP

#include <memory>
#include "maze_loader.hpp"
#include "maze.hpp"
//#include "../client/client_world_manager.hpp"
#include "../client/client_player.hpp"
#include "../client/client_resource.hpp"
#include "../client/client_enemy.hpp"

#include "../server/server_player.hpp"
#include "../server/server_resource.hpp"
#include "../server/server_enemy.hpp"

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

class game_objects_factory
{
public:
    game_objects_factory(std::shared_ptr<presentation::renderer> renderer,
                         std::shared_ptr<control::controller> controller,
                         std::shared_ptr<networking::client> client);
    std::shared_ptr<maze> create_maze(std::shared_ptr<maze_loader> loader);

    std::shared_ptr<client_player> create_client_player(int posx, int posy);
    std::shared_ptr<server_player> create_server_player(int posx, int posy);

    std::shared_ptr<client_enemy> create_client_enemy(
            std::shared_ptr<core::client_world_manager> manager,
            int posx, int posy, int id);
    std::shared_ptr<server_enemy> create_server_enemy(int posx, int posy);

    std::shared_ptr<client_resource> create_client_resource(const std::string &name, int posx, int posy);
    std::shared_ptr<server_resource> create_server_resource(const std::string &name, int posx, int posy);
private:
    std::shared_ptr<presentation::renderer> renderer_;
    std::shared_ptr<control::controller> controller_;
    std::shared_ptr<networking::client> client_;
    std::shared_ptr<core::maze> maze_;
};

}

#endif // GAME_OBJECTS_FACTORY_HPP
