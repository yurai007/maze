#ifndef CLIENT_WORLD_MANAGER_HPP
#define CLIENT_WORLD_MANAGER_HPP

#include <unordered_map>
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

class client_world_manager : public std::enable_shared_from_this<client_world_manager>
{
public:
    client_world_manager(smart::fit_smart_ptr<client_game_objects_factory> objects_factory_,
                         smart::fit_smart_ptr<networking::network_manager> network_manager_,
                         bool automatic_players_);

    void load_all();
    void tick_all();
    void draw_all();
    std::tuple<int, int> get_enemy_position(int id);
    std::tuple<int, int> get_player_position(int id);
    void make_maze(smart::fit_smart_ptr<maze_loader> loader);
    void shut_down_client();

private:

    networking::messages::get_enemies_data_response get_enemies_data_from_network();
    networking::messages::get_players_data_response get_players_data_from_network();
    int get_id_data_from_network();

    void register_player_and_load_external_players_and_enemies_data();
    void load_images_for_drawables() ;
    void handle_external_players_and_enemies();

    void make_enemy(int posx, int posy);
    void make_player(int posx, int posy);
    void make_resource(const std::string &name, int posx, int posy);

    void add_enemy(int posx, int posy, int id);
    void load_image_if_not_automatic(std::shared_ptr<drawable> object);
    // assumption that only one player disappeard which clearly can be wrong
    int remove_absent_player(networking::messages::get_players_data_response &players_data);
    // same as above. Assumption that only one new at time
    std::shared_ptr<drawable> make_external_player(int id, int posx, int posy);

    static const char *bool_to_string(bool x)
    {
        return "false\0true"+6*x;
    }

    std::shared_ptr<core::client_maze> maze {nullptr};
    std::vector<std::shared_ptr<client_player>> players;
    std::vector<std::shared_ptr<client_enemy>> enemies;
    std::vector<std::shared_ptr<client_resource>> resources;

    smart::fit_smart_ptr<client_game_objects_factory> objects_factory;
    smart::fit_smart_ptr<networking::network_manager> network_manager {nullptr};
    std::unordered_map<std::pair<int, int>, int, hash_pair_helper> position_to_enemy_id;
    std::unordered_map<int, std::pair<int, int>> enemy_id_to_position;
    std::unordered_map<std::pair<int, int>, int, hash_pair_helper> position_to_player_id;
    std::map<int, std::pair<int, int>> player_id_to_position;

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
