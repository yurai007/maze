#ifndef SERVER_WORLD_MANAGER_HPP
#define SERVER_WORLD_MANAGER_HPP

#include <vector>
#include <memory>

#include "game_object.hpp"
#include "enemy.hpp"
#include "controller.hpp"
#include "maze.hpp"
#include "logger.hpp"
#include "maze_loader.hpp"
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

class server_world_manager
{
public:
    server_world_manager(std::shared_ptr<presentation::renderer> renderer_,
                  std::shared_ptr<control::controller> controller_,
                  std::shared_ptr<networking::client> client_);
    void add_maze(std::shared_ptr<maze_loader> loader);
    void add_remote_player(int posx, int posy);
    void add_enemy(int posx, int posy);
    void add_resource(const std::string &name, int posx, int posy);
    void load_all();
    void tick_all();
    void tick_all_client();
    void draw_all();

    std::shared_ptr<maze> maze_;

private:
    std::vector<std::shared_ptr<game_object>> game_objects;

    std::shared_ptr<presentation::renderer> renderer;
    std::shared_ptr<control::controller> controller;

    // client data
    std::shared_ptr<networking::client> client;
    std::vector<core::enemy> enemies;
};

}

#endif // SERVER_WORLD_MANAGER_HPP
