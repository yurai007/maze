#ifndef SERVER_WORLD_MANAGER_HPP
#define SERVER_WORLD_MANAGER_HPP

#include <vector>
#include <memory>
#include <unordered_map>
#include "server_game_objects_factory.hpp"
#include "../common/messages.hpp"

using namespace networking;

namespace core
{

class server_world_manager
{
public:
    server_world_manager(std::shared_ptr<server_game_objects_factory>
                         objects_factory_);

    void load_all();
    void tick_all();
    void make_maze(std::shared_ptr<maze_loader> loader);
    std::vector<int> get_enemies_data() const;
    std::shared_ptr<core::server_maze> get_maze() const;
    void update_player_position(int player_id, int oldx, int oldy, int newx, int newy);
    std::vector<int> get_players_data() const;
    int allocate_data_for_new_player();
    void shutdown_player(int id);
    std::pair<int, int> get_player_position(int player_id);

private:

    void load_maze_from_file();
    void make_enemy(int posx, int posy);
    std::shared_ptr<server_player> make_player(int posx, int posy, bool alive);
    void make_resource(const std::string &name, int posx, int posy);

    std::shared_ptr<server_game_objects_factory> objects_factory;
    std::unordered_map<int, std::pair<int, int>> player_id_to_position;

    std::shared_ptr<core::abstract_maze> maze {nullptr};
    std::vector<std::shared_ptr<server_enemy>> enemies;
    std::vector<std::shared_ptr<server_player>> players;
    std::vector<std::shared_ptr<server_resource>> resources;
};

}

#endif // SERVER_WORLD_MANAGER_HPP
