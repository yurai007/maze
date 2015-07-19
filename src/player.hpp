#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <memory>
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
    player(std::shared_ptr<presentation::renderer> renderer_,
           std::shared_ptr<control::controller> controller_,
           std::shared_ptr<core::maze> maze_);
    void load() override;
    void tick() override;
    void draw() override;

private:
    std::shared_ptr<presentation::renderer> renderer;
    std::shared_ptr<control::controller> controller;
    std::shared_ptr<core::maze> maze;
    bool perform_rotation {false};
    char direction;
    int posx {19}, posy {12};
};

}

#endif // PLAYER_HPP
