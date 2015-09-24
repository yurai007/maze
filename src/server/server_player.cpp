#include <cassert>
#include "server_player.hpp"
#include "server_maze.hpp"
#include "server_world_manager.hpp"

namespace core
{

server_player::server_player(std::shared_ptr<core::server_maze> maze_,
                             std::shared_ptr<core::server_world_manager> manager_,
                             int posx_, int posy_)
: game_object(posx_, posy_),
  maze(maze_),
  manager(manager_)
{
    assert(manager != nullptr);
    static int id_generator = 0;
    id_generator++;
    id = id_generator;
}

void server_player::tick(unsigned short)
{
    auto new_position = manager->get_player_position(id);
    posx = new_position.first;
    posy = new_position.second;
}

}
