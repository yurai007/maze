#include <numeric>
#include "client_player.hpp"
#include "renderer.hpp"
#include "client_maze.hpp"
#include "controller.hpp"
#include "network_manager.hpp"

namespace core
{

client_player::client_player(client_world_manager &manager_,
                             smart::fit_smart_ptr<presentation::renderer> renderer_,
                             smart::fit_smart_ptr<control::controller> controller_,
                             smart::fit_smart_ptr<core::client_maze> maze_,
                             smart::fit_smart_ptr<networking::network_manager> network_manager_,
                             int id_, int posx_, int posy_, bool active_, bool automatic_)
    : game_object(posx_, posy_),
      drawable(renderer_),
      manager(manager_),
      controller(controller_),
      maze(maze_),
      network_manager(network_manager_),
      id(id_),
      active(active_),
      automatic(automatic_)
{
    assert(network_manager != nullptr);
}

void client_player::active_tick()
{
    assert(controller != nullptr);

    const int oldx = posx, oldy = posy;

    if (timer_for_escape > 0)
        timer_for_escape--;

    bool enemy_arround = maze->in_range(posx-1, posy) && (maze->get_field(posx-1, posy) == 'E');
    enemy_arround = enemy_arround || (maze->in_range(posx+1, posy) && (maze->get_field(posx+1, posy) == 'E'));
    enemy_arround = enemy_arround || (maze->in_range(posx, posy-1) && (maze->get_field(posx, posy-1) == 'E'));
    enemy_arround = enemy_arround || (maze->in_range(posx, posy+1) && (maze->get_field(posx, posy+1) == 'E'));

    if ((timer_for_escape == 0) && enemy_arround)
    {
        manager.player_health--;
        logger_.log("client_player: contact with enemy, health: %d", manager.player_health);
        if (manager.player_health == 0)
        {
            logger_.log("client_player: died :(");
            died = true;
            return;
        }
        timer_for_escape = 100;
    }

    direction = controller->get_direction();
    posx += (direction == 'L')? -1 : (direction == 'R')? 1 :0;
    posy += (direction == 'U')? -1 : (direction == 'D')? 1 :0;

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

    if (controller->is_space_on())
    {
        auto fposx = posx, fposy = posy;

        if (rotation == 'L')
        {
            fposx--;
        }
        else
            if (rotation == 'R')
            {
                fposx++;
            }
            else
                if (rotation == 'U')
                {
                    fposy--;
                }
                else
                    if (rotation == 'D')
                    {
                        fposy++;
                    }

        if (maze->in_range(fposx, fposy))
        {
            bool answer = network_manager->send_fireball_triggered_over_network(id, fposx, fposy, rotation);
            logger_.log("client_player: fireball request with response: %d", answer);
        }
    }

    controller->reset();
    old_direction = direction;

    if ((oldx != posx) || (oldy != posy))
    {
        const char field = maze->get_field(posx, posy);
        if ( field == 'G' || field == 'M' || field == 'S' || field == 'W' || field == 's')
        {
            // this resource was taken by me
            manager.player_cash++;
            logger_.log("client_player: player cash: %u", manager.player_cash);
        }

        network_manager->send_position_changed_over_network(id, oldx, oldy, posx, posy);
    }
}

void client_player::unactive_tick()
{
    direction = 0;
    if (get_position() == new_position)
        return;

    const int new_x = std::get<0>(new_position);
    const int new_y = std::get<1>(new_position);

    logger_.log_debug("client_player %d: new position = {%d, %d}", id, new_x, new_y);

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
        logger_.log("client_player %d: detected teleportation", id);
        direction = 0;
    }

    posx = new_x;
    posy = new_y;
    perform_rotation = true;
}

void client_player::automatic_tick(int)
{
    assert(maze != nullptr);
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
    const int oldx = posx;
    const int oldy = posy;
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

    if ((oldx != posx) || (oldy != posy))
        network_manager->send_position_changed_over_network(id, oldx, oldy, posx, posy);
}

bool client_player::is_active() const
{
    return active;
}

int client_player::get_id() const
{
    return id;
}

void client_player::new_tick(int new_x, int new_y)
{
    new_position = {new_x, new_y};
}

void client_player::load_image()
{
    assert(renderer != nullptr);
    // TO DO: check this - every player has own image, why?
    unsigned color_id = 1;
    if (!active || automatic)
    {
        color_id = 1 + rand()%6;
    }

    renderer->load_image_and_register("player" + std::to_string(id), "../../../data/player"
                                      + std::to_string(color_id) + ".bmp");
}

void client_player::tick(unsigned short tick_counter)
{
    if (active)
    {
        if (!automatic)
            active_tick();
        else
            automatic_tick(tick_counter);
    }
    else
        unactive_tick();
}

void client_player::draw(int active_player_x, int active_player_y)
{
    assert(renderer != nullptr);
    const std::string image_name = "player" + std::to_string(id);
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
        rotation = direction;
    }
    const int half_width = 50/2;
    const int half_height = 50/2;

    if (active)
        renderer->draw_image(image_name, 30*half_width, 30*half_height);
    else
    {
        const int player_x = posx + half_width - active_player_x;
        const int player_y = posy + half_height - active_player_y;

        if (player_x >= 0 && player_y >= 0)
            renderer->draw_image(image_name, 30*player_x, 30*player_y);
    }
}

}
