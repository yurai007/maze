#ifndef GAME_OBJECT_HPP
#define GAME_OBJECT_HPP

#include <tuple>

namespace core
{

class game_object
{
public:
    virtual void load() = 0;
    virtual void tick(unsigned short tick_counter) = 0;
    virtual void draw() = 0;
    virtual std::tuple<int, int> get_position() const = 0;
    virtual ~game_object() {}
};

}

#endif // GAME_OBJECT_HPP
