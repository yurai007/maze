#ifndef SERVER_WORLD_MANAGER_HPP
#define SERVER_WORLD_MANAGER_HPP

#include <vector>
#include <memory>
#include <unordered_map>
#include "../common/abstract_world_manager.hpp"
#include "server_game_objects_factory.hpp"
#include "../common/messages.hpp"

using namespace networking;

namespace core
{

class server_world_manager : public abstract_world_manager
{
public:
    server_world_manager(std::shared_ptr<server_game_objects_factory>
                         objects_factory_);

    void preprocess_loading() override;
    void postprocess_loading() override;
    void preprocess_ticking() override;

    void make_maze(std::shared_ptr<maze_loader> loader) override;
    void make_enemy(int posx, int posy) override;
    void make_player(int posx, int posy) override;
    void make_resource(const std::string &name, int posx, int posy) override;
    bool check_if_resource(std::shared_ptr<game_object> object) override;

    std::vector<int> get_enemies_data(bool verify) const;
    std::shared_ptr<core::server_maze> get_maze() const;
    void update_player_position(int player_id, int oldx, int oldy, int newx, int newy);
    messages::get_players_data_response allocate_player_for_client();
    std::pair<int, int> get_player_position(int player_id);

private:
    std::shared_ptr<server_game_objects_factory> objects_factory;
    std::unordered_map<int, std::pair<int, int>> player_id_to_position;
};

}

#endif // SERVER_WORLD_MANAGER_HPP
