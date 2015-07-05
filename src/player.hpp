#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "creature.hpp"

namespace presentation
{
    class renderer;
}

namespace control
{
    class controller;
}

namespace core
{
    class maze;
}

namespace core
{

class player : public creature
{
public:
    player(presentation::renderer *renderer_,
           control::controller *controller_,
           core::maze *maze_);
    void load();
    void tick() override;
    void draw() override;
private:
    presentation::renderer *renderer;
    control::controller *controller;
    core::maze *maze;
};

}

#endif // PLAYER_HPP
