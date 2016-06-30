#ifndef GAME_OBJECT_HPP
#define GAME_OBJECT_HPP

#include <tuple>
#include "smart_ptr.hpp"

namespace core
{

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
