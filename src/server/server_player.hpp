#ifndef SERVER_PLAYER_HPP
#define SERVER_PLAYER_HPP

#include <memory>
#include "../common/game_object.hpp"

namespace core
{
    class server_maze;
    class server_world_manager;
}

namespace core
{

class server_player : public game_object
{
public:
    server_player(std::shared_ptr<core::server_maze> maze_,
                  std::shared_ptr<core::server_world_manager> manager_,
                  int posx_, int posy_);

    void tick(unsigned short tick_counter) override;

//private:
    std::shared_ptr<core::server_maze> maze;
    std::shared_ptr<core::server_world_manager> manager;
    int id;
    bool alive {false};
};

}

#endif // SERVER_PLAYER_HPP
