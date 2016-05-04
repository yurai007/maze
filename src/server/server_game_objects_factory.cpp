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
        smart::fit_smart_ptr<std::unordered_map<int, std::pair<int, int>>> positions_cache,
        int posx, int posy, bool alive)
{
    return smart::smart_make_shared<server_player>(maze_, positions_cache, posx, posy, alive);
}

smart::fit_smart_ptr<server_enemy> server_game_objects_factory::create_server_enemy(
        int posx, int posy)
{
    return smart::smart_make_shared<server_enemy>(maze_, posx, posy);
}

smart::fit_smart_ptr<server_resource> server_game_objects_factory::create_server_resource(
        const std::string &name, int posx, int posy)
{
    return smart::smart_make_shared<server_resource>(name, posx, posy);
}

}
