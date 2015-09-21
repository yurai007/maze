#ifndef SERVER_PLAYER_HPP
#define SERVER_PLAYER_HPP

#include <memory>
#include "../common/game_object.hpp"

namespace core
{
    class maze;
}

namespace core
{

class server_player : public game_object
{
public:
    server_player(std::shared_ptr<core::maze> maze_, int posx_, int posy_);

    void tick(unsigned short tick_counter) override;

private:
    std::shared_ptr<core::maze> maze;
};

}

#endif // SERVER_PLAYER_HPP
