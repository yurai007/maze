#ifndef CLIENT_PLAYER_HPP
#define CLIENT_PLAYER_HPP

#include <memory>
#include "../common/game_object.hpp"

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
    client_player(
           std::shared_ptr<core::client_world_manager> manager_,
           std::shared_ptr<presentation::renderer> renderer_,
           std::shared_ptr<control::controller> controller_,
           std::shared_ptr<core::client_maze> maze_,
           std::shared_ptr<networking::client> client_,
           int id_,
           int posx_, int posy_,
           bool active_);

    void active_tick();
    void unactive_tick();
    void tick(unsigned short tick_counter) override;

    void draw() override;
    void load_image() override;
private:
    std::shared_ptr<core::client_world_manager> manager;
    std::shared_ptr<control::controller> controller;
    std::shared_ptr<core::client_maze> maze;
    std::shared_ptr<networking::client> client;
    bool perform_rotation {false};
    char direction;
    int id;
    bool active;
};

}

#endif // CLIENT_PLAYER_HPP
