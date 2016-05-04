#ifndef GAME_OBJECT_HPP
#define GAME_OBJECT_HPP

#include <tuple>
#include <memory>

namespace presentation
{
    class renderer;
}

namespace core
{

class drawable
{
public:
    drawable(std::shared_ptr<presentation::renderer> renderer_)
        : renderer(renderer_) {}

    virtual void load_image() = 0;
    virtual void draw(int active_player_x, int active_player_y) = 0;
    virtual ~drawable() {}
protected:
    std::shared_ptr<presentation::renderer> renderer;
};

class game_object
{
public:
    game_object(int posx_, int posy_)
        : posx(posx_), posy(posy_) {}

    virtual void tick(unsigned short tick_counter) = 0;

    std::tuple<int, int> get_position() const
    {
        return std::make_tuple(posx, posy);
    }
    virtual ~game_object() {}

protected:
    int posx, posy;
};

}

#endif // GAME_OBJECT_HPP
