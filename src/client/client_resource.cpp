#include "client_resource.hpp"
#include "renderer.hpp"

namespace core
{

client_resource::client_resource(const std::string &name_,
                   std::shared_ptr<presentation::renderer> renderer_, int posx_, int posy_)
    : game_object(posx_, posy_),
      drawable(renderer_),
      name(name_)
{
}

void client_resource::load_image()
{
    const std::string path = "../../../data/" + name + ".bmp";
    renderer->load_image_and_register(name, path);
}

void client_resource::tick(unsigned short)
{
}


// TO DO: 50 ---> 60 ??!!
void client_resource::draw(int active_player_x, int active_player_y)
{
    constexpr int half_width = 50/2;
    constexpr int half_height = 50/2;
    const int resource_x = posx + half_width - active_player_x;
    const int resource_y = posy + half_height - active_player_y;
    if (resource_x >= 0 && resource_y >= 0)
        renderer->draw_image(name, 30*resource_x, 30*resource_y);
}

}
