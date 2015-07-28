#include "enemy.hpp"
#include "renderer.hpp"
#include "maze.hpp"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iostream>

namespace core
{

enemy::enemy(std::shared_ptr<presentation::renderer> renderer_,
             std::shared_ptr<core::maze> maze_, int posx_, int posy_)
    : renderer(renderer_),
      maze(maze_),
      posx(posx_),
      posy(posy_)
{
    srand(time(NULL));
}

void enemy::load()
{
    renderer->load_image_and_register("enemy", "../../data/enemy.bmp");
}

void enemy::tick(unsigned short tick_counter)
{
    if (tick_counter % 10 != 0)
        return;

    std::vector<char> is_proper = {0, 0, 0, 0};
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
    if (current == 0)
        posx--;
    if (current == 1)
        posx++;
    if (current == 2)
        posy--;
    if (current == 3)
        posy++;
}

void enemy::draw()
{
    renderer->draw_image("enemy", 30*posx, 30*posy);
}

std::tuple<int, int> enemy::get_position() const
{
    return std::make_tuple(posx, posy);
}

}
