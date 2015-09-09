#include "remote_player.hpp"
#include "renderer.hpp"
#include "controller.hpp"
#include "maze.hpp"
#include "messages.hpp"

namespace core
{

remote_player::remote_player(std::shared_ptr<presentation::renderer> renderer_,
                             std::shared_ptr<control::controller> controller_,
                             std::shared_ptr<core::maze> maze_,
                             std::shared_ptr<networking::client> client_,
                             int posx_, int posy_)
    : renderer(renderer_),
      controller(controller_),
      maze(maze_),
      client(client_),
      posx(posx_),
      posy(posy_)
{
}

void remote_player::load()
{
     renderer->load_image_and_register("player", "../../../data/player.bmp");
}

void remote_player::tick(unsigned short)
{
    if (controller == nullptr)
        return;

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
    if (client != nullptr && perform_rotation)
    {
        networking::messages::position_changed request = {oldx, oldy, posx, posy};
        client->send_request(request);
        auto response = client->read_position_changed_response();
        assert(response.content == "OK");
        logger_.log("remote_player: send position_changed request = {%d, %d} -> {%d, %d} "
                    "and got response", oldx, oldy, posx, posy);
    }
}

void remote_player::draw()
{
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

std::tuple<int, int> remote_player::get_position() const
{
    return std::make_tuple(posx, posy);
}

}
