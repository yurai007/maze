#include "server_player.hpp"
#include "maze.hpp"

namespace core
{

server_player::server_player(std::shared_ptr<core::maze> maze_, int posx_, int posy_)
:
  maze(maze_),
  posx(posx_),
  posy(posy_)
{
}

void server_player::load()
{
}

void server_player::tick(unsigned short)
{
}

void server_player::draw()
{
}

std::tuple<int, int> server_player::get_position() const
{
    return std::make_tuple(posx, posy);
}

}
