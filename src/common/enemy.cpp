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
      posy(posy_),
      id(0)
{
    static int id_generator = 0;
    id_generator++;
    id = id_generator;
    srand(time(NULL));
}

void enemy::load()
{
    renderer->load_image_and_register("enemy" + std::to_string(id), "../../../data/enemy.bmp");
}

void enemy::tick(unsigned short tick_counter)
{
    /* for small number of enemies:
            client: get_enemy(id)_pos??
       otherwise:
            client: get_chunk for all visible chunks
    */

    if (tick_counter % 10 != 0)
        return;

    direction = 0;
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
    perform_rotation = true;
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
}

void enemy::draw()
{
    std::string image_name = "enemy" + std::to_string(id);
    if (perform_rotation)
    {
        if (direction == 'L')
            renderer->rotate_image(image_name, presentation::clockwise_rotation::d270);
        if (direction == 'R')
            renderer->rotate_image(image_name, presentation::clockwise_rotation::d90);
        if (direction == 'D')
            renderer->rotate_image(image_name, presentation::clockwise_rotation::d180);
        if (direction == 'U')
            renderer->rotate_image(image_name, presentation::clockwise_rotation::d360);
    }
    renderer->draw_image(image_name, 30*posx, 30*posy);
}

std::tuple<int, int> enemy::get_position() const
{
    return std::make_tuple(posx, posy);
}

}
