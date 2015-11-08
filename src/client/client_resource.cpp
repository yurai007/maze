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

void client_resource::draw()
{
    renderer->draw_image(name, 30*posx, 30*posy);
}

}
