#ifndef CREATURE_HPP
#define CREATURE_HPP

#include "game_object.hpp"

namespace core
{

class creature : public game_object
{
public:
    void tick(unsigned short tick_counter) override;
    void draw() override;
};

}

#endif // CREATURE_HPP
