#ifndef CLIENT_WORLD_MANAGER_HPP
#define CLIENT_WORLD_MANAGER_HPP

#include <unordered_map>
#include <utility>
#include "../common/abstract_world_manager.hpp"
#include "client.hpp"
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

class client_world_manager : public abstract_world_manager,
                             public std::enable_shared_from_this<client_world_manager>
{
public:
    client_world_manager(std::shared_ptr<client_game_objects_factory> objects_factory_,
                         std::shared_ptr<networking::client> client_);
    networking::messages::get_enemies_data_response get_enemies_data_from_network();
    networking::messages::get_players_data_response get_players_data_from_network();
    int get_id_data_from_network();

    void preprocess_loading() override;
    void postprocess_loading() override;
    void preprocess_ticking() override;
    void make_maze(std::shared_ptr<maze_loader> loader) override;
    void make_enemy(int posx, int posy) override;
    void make_player(int posx, int posy) override;
    void make_resource(const std::string &name, int posx, int posy) override;
    bool check_if_resource(std::shared_ptr<game_object> object) override;

    void draw_all();
    void add_enemy(int posx, int posy, int id);
    std::tuple<int, int> get_enemy_position(int id);
    std::tuple<int, int> get_player_position(int id);

private:
    std::shared_ptr<client_game_objects_factory> objects_factory;
    std::shared_ptr<networking::client> client {nullptr};
    std::unordered_map<std::pair<int, int>, int, hash_pair_helper> position_to_enemy_id;
    std::unordered_map<int, std::pair<int, int>> enemy_id_to_position;
    std::unordered_map<std::pair<int, int>, int, hash_pair_helper> position_to_player_id;
    std::unordered_map<int, std::pair<int, int>> player_id_to_position;

    int player_id {0};
    int player_posx {INT_MAX};
    int player_posy {INT_MAX};
};

}

#endif // CLIENT_WORLD_MANAGER_HPP
