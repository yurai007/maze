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
                  std::weak_ptr<core::server_world_manager> manager_,
                  int posx_, int posy_, bool alive_);

    void tick(unsigned short tick_counter) override;
    bool is_alive() const;
    int get_id() const;

private:
    std::shared_ptr<core::server_maze> maze;
    std::weak_ptr<core::server_world_manager> manager;
    int id;
    bool alive;
};

}

#endif // SERVER_PLAYER_HPP
