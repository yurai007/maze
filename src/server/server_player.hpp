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
                  smart::fit_smart_ptr<std::unordered_map<int, std::pair<int, int>>> player_id_to_pos,
                  int posx_, int posy_, bool alive_);

    void tick(unsigned short tick_counter) override;
    bool is_alive() const;
    int get_id() const;

    void update_player_position(
            int player_id, int oldx, int oldy,
            int newx, int newy);

    std::pair<int, int> get_player_position(
            int player_id) const;

private:
    smart::fit_smart_ptr<core::server_maze> maze;
    smart::fit_smart_ptr<std::unordered_map<int, std::pair<int, int>>> positions_cache;
    int id;
    bool alive;
};

}

#endif // SERVER_PLAYER_HPP
