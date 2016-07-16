#ifndef SERVER_FIREBALL_H
#define SERVER_FIREBALL_H

#include "../common/game_object.hpp"

namespace core
{

class server_fireball : public game_object
{
public:
    server_fireball(int posx_, int posy_, char direction_);
    void tick(unsigned short) override;
private:
    char direction;
};

}

#endif // SERVER_FIREBALL_H
