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

    std::vector<int> get_resources_data() const;
    smart::fit_smart_ptr<core::server_maze> get_maze() const;

    int allocate_data_for_new_player();
    void allocate_data_for_new_fireball(int player_id, int posx, int posy, char direction);
    void generate_resources(unsigned resources_number);
    void shutdown_player(int id);
    void update_player_position(int player_id, int oldx, int oldy, int newx, int newy);
    void repair_if_uncorrect_enemies();
    void repair_if_uncorrect_players();


private:

    static std::string map_field_to_resource_name(const char field);
    static char map_resource_name_to_type(const std::string &name);
    void tick_and_move(smart::fit_smart_ptr<game_object> some_game_object, unsigned short tick_counter);

    void make_maze(smart::fit_smart_ptr<maze_loader> loader);
    void make_enemy(int posx, int posy, int id);
    smart::fit_smart_ptr<server_player> make_player(int posx, int posy, bool alive, int id);
    void make_resource(const std::string &name, int posx, int posy);
    void make_fireball(int player_id, int posx, int posy, char direction);

    smart::fit_smart_ptr<server_game_objects_factory> objects_factory;
    smart::fit_smart_ptr<core::server_maze> maze {nullptr};
    std::vector<smart::fit_smart_ptr<server_enemy>> enemies;
    std::vector<smart::fit_smart_ptr<server_player>> players;
    std::vector<smart::fit_smart_ptr<server_resource>> resources;
    std::vector<smart::fit_smart_ptr<server_fireball>> fireballs;

    int last_player_id {0};
};

}

#endif // SERVER_WORLD_MANAGER_HPP
