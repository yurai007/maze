#ifndef CLIENT_PLAYER_HPP
#define CLIENT_PLAYER_HPP

#include <memory>
#include "creature.hpp"
// TODO: this dependency direction (into client) must be removed
#include "../client/client.hpp"

namespace presentation
{
    class renderer;
}

namespace control
{
    class controller;
}

namespace core
{
    class maze;
}

namespace core
{

class client_player : public creature
{
public:
    client_player(std::shared_ptr<presentation::renderer> renderer_,
           std::shared_ptr<control::controller> controller_,
           std::shared_ptr<core::maze> maze_,
           std::shared_ptr<networking::client> client_,
           int posx_, int posy_);
    void load() override;
    void tick(unsigned short tick_counter) override;
    void draw() override;
    std::tuple<int, int> get_position() const override;

private:
    std::shared_ptr<presentation::renderer> renderer;
    std::shared_ptr<control::controller> controller;
    std::shared_ptr<core::maze> maze;
    std::shared_ptr<networking::client> client;
    bool perform_rotation {false};
    char direction;
    int posx, posy;
};

}

#endif // CLIENT_PLAYER_HPP
