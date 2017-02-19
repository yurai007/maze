#ifndef SERVER_GAME_OBJECTS_FACTORY_HPP
#define SERVER_GAME_OBJECTS_FACTORY_HPP

#include "../common/maze_loader.hpp"
#include "../common/smart_ptr.hpp"

#include "server_player.hpp"
#include "server_enemy.hpp"
#include "server_maze.hpp"
#include "server_fireball.hpp"

namespace core
{

class server_game_objects_factory
{
public:
    server_game_objects_factory() = default;

    smart::fit_smart_ptr<server_maze> create_server_maze(smart::fit_smart_ptr<maze_loader> loader);
    smart::fit_smart_ptr<server_player> create_server_player(
            int posx, int posy, bool alive, int id);
    smart::fit_smart_ptr<server_enemy> create_server_enemy(int posx, int posy, int id);
    smart::fit_smart_ptr<server_fireball> create_server_fireball(int player_id,
                                                            int posx, int posy, char direction);
private:
    smart::fit_smart_ptr<core::server_maze> maze_;
};

}

#endif // SERVER_GAME_OBJECTS_FACTORY_HPP
