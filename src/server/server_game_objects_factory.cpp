#include <cassert>
#include "server_game_objects_factory.hpp"

namespace core
{

void server_game_objects_factory::set_manager(std::shared_ptr<server_world_manager> manager)
{
    manager_ = manager;
}

std::shared_ptr<server_maze> server_game_objects_factory::create_server_maze(
        std::shared_ptr<maze_loader> loader)
{
    return (maze_ = std::make_shared<server_maze>(loader));
}

std::shared_ptr<server_player> server_game_objects_factory::create_server_player(int posx, int posy,
                                                                                 bool alive)
{
    assert(manager_ != nullptr);
    return std::make_shared<server_player>(maze_, manager_, posx, posy, alive);
}

std::shared_ptr<server_enemy> server_game_objects_factory::create_server_enemy(
        int posx, int posy)
{
    return std::make_shared<server_enemy>(maze_, posx, posy);
}

std::shared_ptr<server_resource> server_game_objects_factory::create_server_resource(
        const std::string &name, int posx, int posy)
{
    return std::make_shared<server_resource>(name, posx, posy);
}

}
