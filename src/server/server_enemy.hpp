#ifndef SERVER_ENEMY_HPP
#define SERVER_ENEMY_HPP

#include <memory>
#include "../common/game_object.hpp"

namespace core
{
    class server_maze;
}

namespace core
{

class server_enemy : public game_object
{
public:
    server_enemy(std::shared_ptr<core::server_maze> maze_, int posx_, int posy_);
    int get_id() const;
    void tick(unsigned short tick_counter) override;

private:
    std::shared_ptr<core::server_maze> maze;
    char direction;
    bool perform_rotation {false};
    int id;
};

}

#endif // SERVER_ENEMY_HPP
