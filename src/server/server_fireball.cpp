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
    if (freeze)
        return;

    if (internal_counter % 100 == 0)
    {
        if (direction == 'L')
        {
            posx--;
        }
        else
            if (direction == 'R')
            {
                posx++;
            }
            else
                if (direction == 'U')
                {
                    posy--;
                }
                else
                    if (direction == 'D')
                    {
                        posy++;
                    }
    }
    internal_counter++;
}

}

