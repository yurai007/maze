#include "client_player.hpp"
#include "client.hpp"
#include "renderer.hpp"
#include "client_maze.hpp"
#include "../common/controller.hpp"
#include "../common/messages.hpp"

namespace core
{

client_player::client_player(std::shared_ptr<client_world_manager> manager_,
                             std::shared_ptr<presentation::renderer> renderer_,
                             std::shared_ptr<control::controller> controller_,
                             std::shared_ptr<core::client_maze> maze_,
                             std::shared_ptr<networking::client> client_, int id_,
                             int posx_, int posy_, bool active_, bool automatic_)
    : game_object(posx_, posy_),
      drawable(renderer_),
      manager(manager_),
      controller(controller_),
      maze(maze_),
      client(client_),
      id(id_),
      active(active_),
      automatic(automatic_)
{
}

void client_player::active_tick()
{
    assert(controller != nullptr);

    static char old_direction = 0;
    int oldx = posx, oldy = posy;

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
    controller->reset_direction();
    old_direction = direction;

    // ugly spagetti. Refactor that!!
    // client: get_chunk request to server
    // Also core shouldn't has idea about networking existence
    // Refactor too!!

    // networking::network_notifier/proxy notifier/proxy; notifier.send_position_changed_info();

    if (client != nullptr && ((oldx != posx) || (oldy != posy)) )
    {
        networking::messages::position_changed request = {id, oldx, oldy, posx, posy};
        client->send_request(request);
        auto response = client->read_position_changed_response();
        assert(response.content == "OK");
        logger_.log("client_player: id = %d, send position_changed request = {%d, %d} -> {%d, %d} "
                    "and got response", id, oldx, oldy, posx, posy);
    }
}

void client_player::unactive_tick()
{
    auto new_position = manager->get_player_position(id);
    direction = 0;
    if (get_position() == new_position)
        return;

    int new_x = std::get<0>(new_position);
    int new_y = std::get<1>(new_position);

    logger_.log("client_player %d: new position = {%d, %d}", id, new_x, new_y);

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

void client_player::automatic_tick(int tick_counter)
{
    if (tick_counter % 10 != 0)
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

    // ugly spagetti. Refactor that!!
    // client: get_chunk request to server
    // Also core shouldn't has idea about networking existence
    // Refactor too!!

    // networking::network_notifier/proxy notifier/proxy; notifier.send_position_changed_info();

    if (client != nullptr && ((oldx != posx) || (oldy != posy)) )
    {
        networking::messages::position_changed request = {id, oldx, oldy, posx, posy};
        client->send_request(request);
        auto response = client->read_position_changed_response();
        assert(response.content == "OK");
        logger_.log("client_player: id = %d, send position_changed request = {%d, %d} -> {%d, %d} "
                    "and got response", id, oldx, oldy, posx, posy);
    }
}

bool client_player::is_active() const
{
    return active;
}

void client_player::load_image()
{
    assert(renderer != nullptr);
    renderer->load_image_and_register("player" + std::to_string(id), "../../../data/player.bmp");
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

void client_player::draw()
{
    assert(renderer != nullptr);
    std::string image_name = "player" + std::to_string(id);
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
