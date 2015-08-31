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
    player(std::shared_ptr<core::maze> maze_,
           int posx_, int posy_);
    void load() override;
    void tick(unsigned short tick_counter) override;
    void draw() override;
    std::tuple<int, int> get_position() const override;

private:
    std::shared_ptr<core::maze> maze;
    int posx, posy;
};

}

#endif // PLAYER_HPP
