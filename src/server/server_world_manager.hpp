#ifndef SERVER_WORLD_MANAGER_HPP
#define SERVER_WORLD_MANAGER_HPP

#include <vector>
#include <unordered_map>
#include "server_game_objects_factory.hpp"
#include "../common/messages.hpp"
#include "../common/smart_ptr.hpp"

using namespace networking;

namespace core
{

class server_world_manager
{
public:
    server_world_manager(smart::fit_smart_ptr<server_game_objects_factory> objects_factory_);

    void load_all(smart::fit_smart_ptr<maze_loader> loader);
    void tick_all();
    smart::fit_smart_ptr<core::server_maze> get_maze() const;

    int allocate_data_for_new_player();
    bool allocate_new_fireball_if_possible(int player_id, int posx, int posy, char direction);
    void generate_resources(unsigned resources);
    void shutdown_player(int id);
    void update_player_position(int player_id, int oldx, int oldy, int newx, int newy);
    void repair_if_uncorrect_enemies();
    void repair_if_uncorrect_players();


private:
    void remove_resource(auto pos);
    void tick_and_move(smart::fit_smart_ptr<game_object> some_game_object, unsigned short tick_counter);

    smart::fit_smart_ptr<server_game_objects_factory> objects_factory;
    smart::fit_smart_ptr<core::server_maze> maze {nullptr};
    std::vector<smart::fit_smart_ptr<server_enemy>> enemies;
    std::vector<smart::fit_smart_ptr<server_player>> players;
    std::vector<smart::fit_smart_ptr<server_fireball>> fireballs;

    unsigned resources_number {0};
    int last_player_id {0};
    int last_fireball_id {0};
};

}

#endif // SERVER_WORLD_MANAGER_HPP
