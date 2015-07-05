#include "player.hpp"
#include "renderer.hpp"
#include "controller.hpp"
#include "maze.hpp"

namespace core
{

player::player(presentation::renderer *renderer_,
               control::controller *controller_,
               core::maze *maze_)
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

    int oldx = posx;
    int oldy = posy;

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
    }
    else
    if ((old_direction != direction) && (direction != 0))
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

    old_direction = direction;
    controller->reset_direction();
    renderer->draw_image("player", 30*posx, 30*posy);
}

}
