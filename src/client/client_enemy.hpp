#ifndef CLIENT_ENEMY_HPP
#define CLIENT_ENEMY_HPP

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
    client_enemy(smart::fit_smart_ptr<presentation::renderer> renderer_,
                 int posx_, int posy_, int id_);

    void new_tick(int new_x, int new_y);
    void load_image() override;
    void draw(int active_player_x, int active_player_y) override;

private:
    void tick(unsigned short) override;
    std::tuple<int, int> new_position;

    char direction;
    bool perform_rotation {false};
    int id;
};

}

#endif // CLIENT_ENEMY_HPP
