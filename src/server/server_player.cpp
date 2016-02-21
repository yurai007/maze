#include <cassert>
#include "../common/logger.hpp"
#include "server_player.hpp"
#include "server_maze.hpp"
#include "server_world_manager.hpp"

namespace core
{

server_player::server_player(std::shared_ptr<core::server_maze> maze_,
                             std::shared_ptr<core::server_world_manager> manager_,
                             int posx_, int posy_, bool alive_)
    : game_object(posx_, posy_),
      maze(maze_),
      manager(manager_),
      alive(alive_)
{
    assert(manager != nullptr);
    static int id_generator = 0;
    id_generator++;
    id = id_generator;
    manager->update_player_position(id, posx, posy, posx, posy);
}

void server_player::tick(unsigned short)
{
    auto new_position = manager->get_player_position(id);
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

}
