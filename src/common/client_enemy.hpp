#ifndef CLIENT_ENEMY_HPP
#define CLIENT_ENEMY_HPP

#include <memory>
#include "creature.hpp"

namespace presentation
{
    class renderer;
}

namespace core
{
    class maze;
    class abstract_world_manager;
    class client_world_manager;
}

namespace core
{

class client_enemy : public creature
{
public:
    client_enemy(std::shared_ptr<core::abstract_world_manager> manager_,
                 std::shared_ptr<presentation::renderer> renderer_,
                 std::shared_ptr<core::maze> maze_,
                 int posx_, int posy_, int id_);
    void load() override;
    void tick(unsigned short) override;
    void draw() override;
    std::tuple<int, int> get_position() const override;

private:
    std::shared_ptr<core::client_world_manager> manager;
    std::shared_ptr<presentation::renderer> renderer;
    std::shared_ptr<core::maze> maze;
    char direction;
    int posx, posy;
    bool perform_rotation {false};
    int id;
};

}

#endif // CLIENT_ENEMY_HPP
