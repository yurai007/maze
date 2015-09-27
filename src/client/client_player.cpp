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
                             int posx_, int posy_, bool active_)
    : game_object(posx_, posy_),
      drawable(renderer_),
      manager(manager_),
      controller(controller_),
      maze(maze_),
      client(client_),
      id(id_),
      active(active_)
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

void client_player::load_image()
{
    renderer->load_image_and_register("player" + std::to_string(id), "../../../data/player.bmp");
}

void client_player::tick(unsigned short)
{
    if (active)
        active_tick();
    else
        unactive_tick();
}

void client_player::draw()
{
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
