#include "server_player.hpp"
#include "../common/maze.hpp"

namespace core
{

server_player::server_player(std::shared_ptr<core::maze> maze_, int posx_, int posy_)
: game_object(posx_, posy_),
  maze(maze_)
{
}

void server_player::tick(unsigned short)
{
}

}
