#include "client_enemy.hpp"
#include "renderer.hpp"
#include "maze.hpp"
#include <algorithm>
#include <iostream>

namespace core
{

client_enemy::client_enemy(std::shared_ptr<abstract_world_manager> manager_,
        std::shared_ptr<presentation::renderer> renderer_,
             std::shared_ptr<core::maze> maze_,
             int posx_, int posy_, int id_)
    : manager(nullptr),
      renderer(renderer_),
      maze(maze_),
      posx(posx_),
      posy(posy_),
      id(id_)
{
    // hacky downcasting, remove it in the future
    manager = std::dynamic_pointer_cast<client_world_manager>(manager_);
    assert(manager != nullptr);
}

void client_enemy::load()
{
    renderer->load_image_and_register("enemy" + std::to_string(id), "../../../data/enemy.bmp");
}

void client_enemy::tick(unsigned short )
{
    auto new_position = manager->get_enemy_position(id);
    direction = 0;
    if (get_position() == new_position)
        return;

    int new_x = std::get<0>(new_position);
    int new_y = std::get<1>(new_position);

    logger_.log("client_enemy %d: new position = {%d, %d}", id, new_x, new_y);
    // I assume no lags
    assert( (new_x - posx == 0 ) || (new_y - posy == 0) );

    // I assume no lags again
    if (new_x == posx-1)
        direction = 'L';
    else
    if (new_x == posx+1)
        direction = 'R';
    else
    if (new_y == posy-1)
        direction = 'U';
    else
    if (new_y == posy+1)
        direction = 'D';
    else
        assert(false);

    posx = new_x;
    posy = new_y;
    perform_rotation = true;
}

void client_enemy::draw()
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

std::tuple<int, int> client_enemy::get_position() const
{
    return std::make_tuple(posx, posy);
}

}
