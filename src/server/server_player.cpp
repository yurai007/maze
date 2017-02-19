#include <cassert>
#include "../common/logger.hpp"
#include "server_player.hpp"
#include "server_maze.hpp"
#include "server_world_manager.hpp"

namespace core
{

server_player::server_player(smart::fit_smart_ptr<core::server_maze> maze_,
                             int posx_, int posy_, bool alive_, int id_)
    : game_object(posx_, posy_),
      maze(maze_),
      id(id_),
      alive(alive_)
{
    update_player_position(posx, posy);
}

void server_player::tick(unsigned short)
{
}

bool server_player::is_alive() const
{
    return alive;
}

int server_player::get_id() const
{
    return id;
}

void server_player::update_player_position(int newx, int newy)
{
   //assert(oldx == posx && oldy == posy);
   logger_.log("server_player: id = %d, position changed = {%d, %d} -> {%d, %d}",
                id, posx, posy, newx, newy);
   posx = newx;
   posy = newy;
}

}
