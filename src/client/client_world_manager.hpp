#ifndef CLIENT_WORLD_MANAGER_HPP
#define CLIENT_WORLD_MANAGER_HPP

#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <map>
#include "network_manager.hpp"
#include "client_game_objects_factory.hpp"

namespace core
{

class client_world_manager
{
public:
    client_world_manager(smart::fit_smart_ptr<client_game_objects_factory> objects_factory_,
                         smart::fit_smart_ptr<networking::network_manager> network_manager_,
                         bool automatic_players_);

    void load_all();
    void tick_all();
    void draw_all();
    void make_maze(smart::fit_smart_ptr<maze_loader> loader);
    void shut_down_client();

    unsigned player_cash {0};

private:
    void update_enemies();
    void update_players();
    void update_resources();
    int get_id_data_from_network();

    void load_images_for_drawables();
    void make_enemy(int posx, int posy);
    void make_player(int posx, int posy);
    void make_resource(const char field, int posx, int posy);

    void add_enemy(int posx, int posy, int id);

    static const char *bool_to_string(bool x)
    {
        return "false\0true"+6*x;
    }

    smart::fit_smart_ptr<core::client_maze> maze {nullptr};
    smart::fit_smart_ptr<client_game_objects_factory> objects_factory;
    smart::fit_smart_ptr<networking::network_manager> network_manager {nullptr};

    std::map<int, smart::fit_smart_ptr<client_enemy>> id_to_enemy;
    std::map<int, smart::fit_smart_ptr<client_player>> id_to_player;
    std::map<std::tuple<int, int>, smart::fit_smart_ptr<client_resource>> resources;

    int player_id {0};
    bool automatic_players {false};
    unsigned short tick_counter {0};

};

}

#endif // CLIENT_WORLD_MANAGER_HPP
