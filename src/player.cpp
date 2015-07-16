#include "player.hpp"
#include "renderer.hpp"
#include "controller.hpp"
#include "maze.hpp"

namespace core
{

player::player(std::shared_ptr<presentation::renderer> renderer_,
               std::shared_ptr<control::controller> controller_,
               std::shared_ptr<core::maze> maze_)
: renderer(renderer_),
  controller(controller_),
  maze(maze_)
{
}

void player::load()
{
     renderer->load_image_and_register("player", "../../data/player.bmp");
}

void player::tick()
{

}

void player::draw()
{
    static int posx = 19;
    static int posy = 12;

    static char old_direction = 0;

    int oldx = posx, oldy = posy;

    char direction = controller->get_direction();

    if (direction == 'L')
        posx -= 1;
    else
        if (direction == 'R')
            posx += 1;
        else
            if (direction == 'U')
                posy -= 1;
            else
                if (direction == 'D')
                    posy += 1;

    if (maze->is_field_filled(posx, posy))
    {
        posx = oldx;
        posy = oldy;
        perform_rotation = false;
    }
    else
    if ((old_direction != direction) && (direction != 0))
        perform_rotation = true;
    else
        perform_rotation = false;
    controller->reset_direction();
    old_direction = direction;

    if (perform_rotation)
    {
        if (direction == 'L')
            renderer->rotate_image("player", presentation::clockwise_rotation::d270);
        if (direction == 'R')
            renderer->rotate_image("player", presentation::clockwise_rotation::d90);
        if (direction == 'D')
            renderer->rotate_image("player", presentation::clockwise_rotation::d180);
        if (direction == 'U')
            renderer->rotate_image("player", presentation::clockwise_rotation::d360);
    }
    renderer->draw_image("player", 30*posx, 30*posy);
}

}
