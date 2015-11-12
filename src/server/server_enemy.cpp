#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <array>
#include <iostream>
#include "../common/logger.hpp"
#include "server_enemy.hpp"
#include "server_maze.hpp"

namespace core
{

server_enemy::server_enemy(std::shared_ptr<core::server_maze> maze_,
             int posx_, int posy_)
    : game_object(posx_, posy_),
      maze(maze_),
      id(0)
{
    static int id_generator = 0;
    id_generator++;
    id = id_generator;
    srand(time(NULL));
}

int server_enemy::get_id() const
{
    return id;
}

void server_enemy::tick(unsigned short tick_counter)
{
    if (tick_counter % (10*30) != 0)
        return;

    direction = 0;
    std::array<char, 4> is_proper = {0, 0, 0, 0};
    is_proper[0] += (int)!maze->is_field_filled(posx-1, posy);
    is_proper[1] += (int)!maze->is_field_filled(posx+1, posy);
    is_proper[2] += (int)!maze->is_field_filled(posx, posy-1);
    is_proper[3] += (int)!maze->is_field_filled(posx, posy+1);
    int proper_directions_number = std::accumulate(is_proper.begin(), is_proper.end(), 0);

    if (proper_directions_number == 0)
        return;
    int where = rand()%proper_directions_number;

    int current = 0;
    while (where >= 0)
        if (is_proper[current++])
            where--;
    current--;
    perform_rotation = true;
    int oldx = posx;
    int oldy = posy;
    if (current == 0)
    {
        direction = 'L';
        posx--;
    }
    if (current == 1)
    {
        direction = 'R';
        posx++;
    }
    if (current == 2)
    {
        direction = 'U';
        posy--;
    }
    if (current == 3)
    {
        direction = 'D';
        posy++;
    }

    logger_.log("server_enemy: id = %d, position changed = {%d, %d} -> {%d, %d}",
                id, oldx, oldy, posx, posy);
}

}
