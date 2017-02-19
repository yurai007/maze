#ifndef CLIENT_PLAYER_HPP
#define CLIENT_PLAYER_HPP

#include "../common/smart_ptr.hpp"
#include "../common/game_object.hpp"
#include "network_manager.hpp"
#include "drawable.hpp"
#include "controller.hpp"

namespace control
{
    class controller;
}

namespace networking
{
    class client;
}

namespace core
{
    class client_world_manager;
    class client_maze;
}

namespace core
{

class client_player : public game_object, public drawable
{
public:
    client_player(client_world_manager &manager_,
           smart::fit_smart_ptr<presentation::renderer> renderer_,
           smart::fit_smart_ptr<control::controller> controller_,
           smart::fit_smart_ptr<core::client_maze> maze_,
           smart::fit_smart_ptr<networking::network_manager> network_manager_,
           int id_,
           int posx_, int posy_,
           bool active_,
           bool automatic_);

    void active_tick();
    void unactive_tick();
    void automatic_tick(int tick_counter);
    bool is_active() const;
    int get_id() const;

    void new_tick(int new_x, int new_y);
    void tick(unsigned short tick_counter) override;

    void draw(int active_player_x, int active_player_y) override;
    void load_image() override;

    bool died {false};

private:

    core::client_world_manager &manager;
    smart::fit_smart_ptr<control::controller> controller;
    smart::fit_smart_ptr<core::client_maze> maze;
    smart::fit_smart_ptr<networking::network_manager> network_manager;
    bool perform_rotation {false};
    char direction;
    char old_direction {0};
    int id;
    char rotation {0};

    bool active, automatic;
    unsigned timer_for_escape {0};

    std::tuple<int, int> new_position;
};

}

#endif // CLIENT_PLAYER_HPP
