#include <algorithm>
#include <iostream>
#include "client_enemy.hpp"
#include "renderer.hpp"
#include "client_maze.hpp"

namespace core
{

client_enemy::client_enemy(std::shared_ptr<client_world_manager> manager_,
                           std::shared_ptr<presentation::renderer> renderer_,
                           std::shared_ptr<core::client_maze> maze_,
                           int posx_, int posy_, int id_)
    : game_object(posx_, posy_),
      drawable(renderer_),
      manager(manager_),
      maze(maze_),
      id(id_)
{
    assert(manager != nullptr);
}

void client_enemy::load_image()
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

    //logger_.log("client_enemy %d: new position = {%d, %d}", id, new_x, new_y);

    // I assume lags and teleportation:)
    //assert( (new_x - posx == 0 ) || (new_y - posy == 0) );

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
    {
        assert(true);
        logger_.log("client_enemy %d: detected teleportation", id);
        direction = 0;
    }

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

}
