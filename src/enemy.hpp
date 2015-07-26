#ifndef ENEMY_HPP
#define ENEMY_HPP

#include <memory>
#include "creature.hpp"

namespace presentation
{
    class renderer;
}

namespace core
{
    class maze;
}

namespace core
{

class enemy : public creature
{
public:
    enemy(std::shared_ptr<presentation::renderer> renderer_,
          std::shared_ptr<core::maze> maze_,
          int posx_, int posy_);
    void load() override;
    void tick() override;
    void draw() override;
    std::tuple<int, int> get_position() const override;

private:
    std::shared_ptr<presentation::renderer> renderer;
    std::shared_ptr<core::maze> maze;
    char direction;
    int posx, posy;
};

}

#endif // ENEMY_HPP
