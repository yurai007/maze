#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

#include "../common/smart_ptr.hpp"

namespace presentation
{
    class renderer;
}

namespace core
{

class drawable
{
public:
    drawable(smart::fit_smart_ptr<presentation::renderer> renderer_)
        : renderer(renderer_) {}

    virtual void load_image() = 0;
    virtual void draw(int active_player_x, int active_player_y) = 0;
    virtual ~drawable() {}
protected:
   smart::fit_smart_ptr<presentation::renderer> renderer;
};

}

#endif // DRAWABLE_HPP

