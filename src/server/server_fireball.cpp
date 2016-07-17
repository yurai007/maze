#include "server_fireball.hpp"

namespace core
{

server_fireball::server_fireball(int posx_, int posy_, char direction_)
    : game_object(posx_, posy_),
      direction(direction_)
{
}

void server_fireball::tick(unsigned short)
{

}

}

