#ifndef CLIENT_PLAYER_HPP
#define CLIENT_PLAYER_HPP

#include <memory>
#include "../common/game_object.hpp"

//namespace presentation
//{
//    class renderer;
//}

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
    class maze;
}

namespace core
{

class client_player : public game_object, public drawable
{
public:
    client_player(std::shared_ptr<presentation::renderer> renderer_,
           std::shared_ptr<control::controller> controller_,
           std::shared_ptr<core::maze> maze_,
           std::shared_ptr<networking::client> client_,
           int posx_, int posy_);
    void tick(unsigned short tick_counter) override;

    void draw() override;
    void load_image() override;
private:
    std::shared_ptr<control::controller> controller;
    std::shared_ptr<core::maze> maze;
    std::shared_ptr<networking::client> client;
    bool perform_rotation {false};
    char direction;
};

}

#endif // CLIENT_PLAYER_HPP
