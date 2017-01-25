#ifndef SERVER_PLAYER_HPP
#define SERVER_PLAYER_HPP

#include <unordered_map>
#include "../common/game_object.hpp"
#include "../common/smart_ptr.hpp"

namespace core
{

class server_maze;

class server_player : public game_object
{
public:
    server_player(smart::fit_smart_ptr<core::server_maze> maze_,
                  int posx_, int posy_, bool alive_, int id_);

    void tick(unsigned short tick_counter) override;
    bool is_alive() const;
    int get_id() const;
    void update_player_position(int oldx, int oldy, int newx, int newy);

private:
    smart::fit_smart_ptr<core::server_maze> maze;
    int id;
    bool alive;
};

}

#endif // SERVER_PLAYER_HPP
