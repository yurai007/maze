#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "creature.hpp"

namespace core
{

class enemy : public creature
{
public:
    void tick() override;
    void draw() override;
};

}

#endif // ENEMY_HPP
