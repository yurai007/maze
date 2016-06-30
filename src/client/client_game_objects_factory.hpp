#ifndef CLIENT_GAME_OBJECTS_FACTORY_HPP
#define CLIENT_GAME_OBJECTS_FACTORY_HPP

#include <memory>
#include "../common/maze_loader.hpp"
#include "../common/smart_ptr.hpp"
#include "../client/client_player.hpp"
#include "../client/client_resource.hpp"
#include "../client/client_enemy.hpp"
#include "../client/client_maze.hpp"

namespace presentation
{
    class renderer;
}

namespace control
{
    class controller;
}

namespace networking
{
    class network_manager;
}

namespace core
{

class client_player;
class client_world_manager;

class client_game_objects_factory
{
public:
    client_game_objects_factory(smart::fit_smart_ptr<presentation::renderer> renderer,
                         smart::fit_smart_ptr<control::controller> controller,
                         smart::fit_smart_ptr<networking::network_manager> network_manager);

    std::shared_ptr<client_maze> create_client_maze(smart::fit_smart_ptr<maze_loader> loader,
                                                    bool visible);

    std::shared_ptr<client_player> create_client_player(
                                    std::shared_ptr<client_world_manager> manager,
                                    int id, int posx, int posy,
                                    bool active, bool automatic);

    std::shared_ptr<client_enemy> create_client_enemy(
            std::shared_ptr<core::client_world_manager> manager,
            int posx, int posy, int id);

    std::shared_ptr<client_resource> create_client_resource(
            const std::string &name, int posx, int posy);
private:
    smart::fit_smart_ptr<presentation::renderer> renderer_;
    smart::fit_smart_ptr<control::controller> controller_;
    smart::fit_smart_ptr<networking::network_manager> network_manager_;
    std::shared_ptr<core::client_maze> maze_;
};

}

#endif // CLIENT_GAME_OBJECTS_FACTORY_HPP

