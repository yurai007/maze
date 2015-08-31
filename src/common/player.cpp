#include "player.hpp"
#include "maze.hpp"

namespace core
{

player::player(std::shared_ptr<core::maze> maze_, int posx_, int posy_)
:
  maze(maze_),
  posx(posx_),
  posy(posy_)
{
}

void player::load()
{

}

void player::tick(unsigned short)
{
    // update from maze
}

void player::draw()
{

}

std::tuple<int, int> player::get_position() const
{
    return std::make_tuple(posx, posy);
}

}
