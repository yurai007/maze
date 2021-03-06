#include <algorithm>
#include <iostream>
#include "client_enemy.hpp"
#include "renderer.hpp"
#include "client_maze.hpp"

namespace core
{

client_enemy::client_enemy(smart::fit_smart_ptr<presentation::renderer> renderer_,
                           int posx_, int posy_, int id_)
    : game_object(posx_, posy_),
      drawable(renderer_),
      id(id_)
{
}

void client_enemy::load_image()
{
    renderer->load_image_and_register("enemy" + std::to_string(id), "../../../data/enemy.bmp");
}

void client_enemy::tick(unsigned short )
{
    direction = 0;
    if (get_position() == new_position)
        return;

    int new_x = std::get<0>(new_position);
    int new_y = std::get<1>(new_position);

    logger_.log_debug("client_enemy %d: new position = {%d, %d}", id, new_x, new_y);

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

void client_enemy::new_tick(int new_x, int new_y)
{
    new_position = {new_x, new_y};
    tick(0);
}

void client_enemy::draw(int active_player_x, int active_player_y)
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
    const int half_width = 50/2;
    const int half_height = 50/2;

    const int enemy_x = posx + half_width - active_player_x;
    const int enemy_y = posy + half_height - active_player_y;

    if (enemy_x >= 0 && enemy_y >= 0)
        renderer->draw_image(image_name, 30*enemy_x, 30*enemy_y);
}

}
