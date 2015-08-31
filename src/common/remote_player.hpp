#ifndef REMOTE_PLAYER_HPP
#define REMOTE_PLAYER_HPP

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

class remote_player : public creature
{
public:
    remote_player(std::shared_ptr<presentation::renderer> renderer_,
           std::shared_ptr<control::controller> controller_,
           std::shared_ptr<core::maze> maze_,
           int posx_, int posy_);
    void load() override;
    void tick(unsigned short tick_counter) override;
    void draw() override;
    std::tuple<int, int> get_position() const override;

private:
    std::shared_ptr<presentation::renderer> renderer;
    std::shared_ptr<control::controller> controller;
    std::shared_ptr<core::maze> maze;
    bool perform_rotation {false};
    char direction;
    int posx, posy;
};

}

#endif // REMOTE_PLAYER_HPP