#ifndef CLIENT_ENEMY_HPP
#define CLIENT_ENEMY_HPP

#include <memory>
#include "../common/game_object.hpp"
#include "drawable.hpp"
#include "client_maze.hpp"

namespace presentation
{
    class renderer;
}

namespace core
{
    class client_world_manager;
}

namespace core
{

class client_enemy : public game_object, public drawable
{
public:
    client_enemy(std::shared_ptr<client_world_manager> manager_,
                 smart::fit_smart_ptr<presentation::renderer> renderer_,
                 std::shared_ptr<core::client_maze> maze_,
                 int posx_, int posy_, int id_);
    void tick(unsigned short) override;

    void load_image() override;
    void draw(int active_player_x, int active_player_y) override;
private:
    std::shared_ptr<core::client_world_manager> manager;
    std::shared_ptr<core::client_maze> maze;
    char direction;
    bool perform_rotation {false};
    int id;
};

}

#endif // CLIENT_ENEMY_HPP
