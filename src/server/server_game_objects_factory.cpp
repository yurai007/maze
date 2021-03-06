#include <cassert>
#include "server_game_objects_factory.hpp"

namespace core
{

smart::fit_smart_ptr<server_maze> server_game_objects_factory::create_server_maze(
        smart::fit_smart_ptr<maze_loader> loader)
{
    return (maze_ = smart::smart_make_shared<server_maze>(loader));
}

smart::fit_smart_ptr<server_player> server_game_objects_factory::create_server_player(
        int posx, int posy, bool alive, int id)
{
    assert(maze_ != nullptr);
    return smart::smart_make_shared<server_player>(maze_, posx, posy, alive, id);
}

smart::fit_smart_ptr<server_enemy> server_game_objects_factory::create_server_enemy(
        int posx, int posy, int id)
{
    assert(maze_ != nullptr);
    return smart::smart_make_shared<server_enemy>(maze_, posx, posy, id);
}

smart::fit_smart_ptr<server_fireball> server_game_objects_factory::create_server_fireball(
        int player_id, int posx, int posy, char direction)
{
    (void)player_id;
    return smart::smart_make_shared<server_fireball>(posx, posy, direction);
}

}
