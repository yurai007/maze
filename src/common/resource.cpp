#include "resource.hpp"
#include "renderer.hpp"

namespace core
{

resource::resource(const std::string &name_,
                   std::shared_ptr<presentation::renderer> renderer_, int posx_, int posy_)
    : name(name_),
      renderer(renderer_),
      posx(posx_),
      posy(posy_)
{
}

void resource::load()
{
    renderer->load_image_and_register("gold", "../../data/gold.bmp");
}

void resource::tick(unsigned short)
{
}

void resource::draw()
{
    renderer->draw_image("gold", 30*posx, 30*posy);
}

std::tuple<int, int> resource::get_position() const
{
    return std::make_tuple(posx, posy);
}

}
