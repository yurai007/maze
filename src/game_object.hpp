#ifndef GAME_OBJECT_HPP
#define GAME_OBJECT_HPP

namespace core
{

class game_object
{
public:
    virtual void tick() = 0;
    virtual void draw() = 0;
    virtual ~game_object() {}
};

}

#endif // GAME_OBJECT_HPP
