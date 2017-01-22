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

struct hash_pair_helper
{
public:
    template <typename T, typename U>
    std::size_t operator()(const std::pair<T, U> &pair) const
    {
        return std::hash<T>()(pair.first) ^ std::hash<U>()(pair.second);
    }
};

class client_world_manager
{
public:
    client_world_manager(smart::fit_smart_ptr<client_game_objects_factory> objects_factory_,
                         smart::fit_smart_ptr<networking::network_manager> network_manager_,
                         bool automatic_players_);

    void load_all();
    void tick_all();
    void draw_all();
    std::tuple<int, int> get_player_position(int id);
    void make_maze(smart::fit_smart_ptr<maze_loader> loader);
    void shut_down_client();

    unsigned player_cash {0};

private:

    static std::string map_field_to_resource_name(const char field);

    void update_enemies();
    std::map<int, std::pair<int, int> > get_players();
    networking::messages::get_resources_data_response get_resources_data_from_network();
    int get_id_data_from_network();

    void load_images_for_drawables();
    // this method is needed for filling/updating all game_object containers like: players,
    // resources, etc. Then if all game_object containers are up-to-date we may tick
    void handle_external_dynamic_game_objects();

    void make_enemy(int posx, int posy);
    void make_player(int posx, int posy);
    void make_resource(const std::string &name, int posx, int posy);

    void add_enemy(int posx, int posy, int id);
    void load_image_if_not_automatic(smart::fit_smart_ptr<drawable> object);
    // assumption that only one player disappeared which clearly can be wrong
    int remove_absent_player(std::map<int, std::pair<int, int>> &players_data);
    // same as above. Assumption that only one new at time
    smart::fit_smart_ptr<drawable> make_external_player(int id, int posx, int posy);

    static const char *bool_to_string(bool x)
    {
        return "false\0true"+6*x;
    }

    smart::fit_smart_ptr<core::client_maze> maze {nullptr};
    std::vector<smart::fit_smart_ptr<client_player>> players;
    std::vector<smart::fit_smart_ptr<client_resource>> resources;

    smart::fit_smart_ptr<client_game_objects_factory> objects_factory;
    smart::fit_smart_ptr<networking::network_manager> network_manager {nullptr};

    std::map<int, smart::fit_smart_ptr<client_enemy>> id_to_enemy;
    std::map<int, std::pair<int, int>> player_id_to_position;
    std::unordered_set<std::pair<int, int>, hash_pair_helper> resources_pos;

    int player_id {0};
    int player_posx {INT_MAX};
    int player_posy {INT_MAX};
    bool automatic_players {false};

    int external_player_id {0};
    int external_player_posx {INT_MAX};
    int external_player_posy {INT_MAX};
    unsigned short tick_counter {0};

};

}

#endif // CLIENT_WORLD_MANAGER_HPP
