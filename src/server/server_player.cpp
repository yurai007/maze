#include <cassert>
#include "../common/logger.hpp"
#include "server_player.hpp"
#include "server_maze.hpp"
#include "server_world_manager.hpp"

namespace core
{

server_player::server_player(smart::fit_smart_ptr<core::server_maze> maze_,
                             smart::fit_smart_ptr<std::unordered_map<int, std::pair<int, int>>> player_id_to_pos,
                             int posx_, int posy_, bool alive_)
    : game_object(posx_, posy_),
      maze(maze_),
      positions_cache(player_id_to_pos),
      alive(alive_)
{
    static int id_generator = 0;
    id_generator++;
    id = id_generator;
    update_player_position(id, posx, posy, posx, posy);
}

void server_player::tick(unsigned short)
{
    auto new_position = get_player_position(id);
    int newx = new_position.first;
    int newy = new_position.second;
        if (newx == posx && newy == posy)
            return;
        else
        {
            logger_.log("server_player: id = %d, position changed = {%d, %d} -> {%d, %d}",
                        id, posx, posy, newx, newy);
            posx = newx;
            posy = newy;
        }
}

bool server_player::is_alive() const
{
    return alive;
}

int server_player::get_id() const
{
    return id;
}

void server_player::update_player_position(
        int player_id, int oldx, int oldy,
        int newx, int newy)
{
   assert( ((newx - oldx == 0 ) || (newy - oldy == 0) ) && ("Some lags happened") );
   (*positions_cache)[player_id] = {newx, newy};
}

std::pair<int, int> server_player::get_player_position(
        int player_id) const
{
    const auto player_it = (*positions_cache).find(player_id);
    assert(player_it != (*positions_cache).end());
    return player_it->second;
}

}
