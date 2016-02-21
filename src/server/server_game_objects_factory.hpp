#ifndef SERVER_GAME_OBJECTS_FACTORY_HPP
#define SERVER_GAME_OBJECTS_FACTORY_HPP

#include <memory>
#include "../common/maze_loader.hpp"

#include "server_player.hpp"
#include "server_resource.hpp"
#include "server_enemy.hpp"
#include "server_maze.hpp"

namespace core
{

class server_player;
class server_world_manager;

class server_game_objects_factory
{
public:
    server_game_objects_factory() = default;
    void set_manager(std::shared_ptr<server_world_manager> manager);

    std::shared_ptr<server_maze> create_server_maze(std::shared_ptr<maze_loader> loader);
    std::shared_ptr<server_player> create_server_player(int posx, int posy, bool alive);
    std::shared_ptr<server_enemy> create_server_enemy(int posx, int posy);
    std::shared_ptr<server_resource> create_server_resource(const std::string &name,
                                                            int posx, int posy);
private:
    std::shared_ptr<core::server_maze> maze_;
    std::shared_ptr<core::server_world_manager> manager_;
};

}

#endif // SERVER_GAME_OBJECTS_FACTORY_HPP
