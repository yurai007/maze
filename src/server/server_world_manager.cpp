#include <cassert>
#include <climits>

#include "server_world_manager.hpp"
#include "../common/logger.hpp"

namespace core
{

server_world_manager::server_world_manager(std::shared_ptr<server_game_objects_factory> objects_factory_)
    : abstract_world_manager(),
      objects_factory(objects_factory_)
{
    assert(objects_factory != nullptr);
    logger_.log("server_world_manager: started");
}

void server_world_manager::preprocess_loading()
{
}

void server_world_manager::postprocess_loading()
{
}

void server_world_manager::preprocess_ticking()
{
}

void server_world_manager::make_maze(std::shared_ptr<maze_loader> loader)
{
    maze_ = objects_factory->create_server_maze(loader);
    logger_.log("server_world_manager: added maze");
}

void server_world_manager::make_enemy(int posx, int posy)
{
    assert(maze_ != nullptr);
    game_objects.push_back(objects_factory->create_server_enemy(posx, posy));
    logger_.log("server_world_manager: added enemy on position = {%d, %d}", posx, posy);
}

void server_world_manager::make_player(int posx, int posy)
{
    game_objects.push_back(objects_factory->create_server_player(posx, posy));
    logger_.log("server_world_manager: added player on position = {%d, %d}", posx, posy);
}

void server_world_manager::make_resource(const std::string &name, int posx, int posy)
{
    game_objects.push_back(objects_factory->create_server_resource(name, posx, posy));
    logger_.log("server_world_manager: added %s on position = {%d, %d}", name.c_str(), posx, posy);
}

bool server_world_manager::check_if_resource(std::shared_ptr<game_object> object)
{
    return std::dynamic_pointer_cast<server_resource>(object) != nullptr;
}

std::vector<int> server_world_manager::get_enemies_data(bool verify) const
{
    std::vector<int> result;
    for (auto &object : game_objects)
    {
        auto enemy = std::dynamic_pointer_cast<server_enemy>(object);
        if (enemy != nullptr)
        {
            auto position = enemy->get_position();

            if (verify)
            {
                char field = maze_->get_field(std::get<0>(position), std::get<1>(position));
                assert(field == 'E');
            }

            result.push_back(enemy->get_id());
            result.push_back(std::get<0>(position));
            result.push_back(std::get<1>(position));
        }
    }
    return result;
}

std::shared_ptr<server_maze> server_world_manager::get_maze() const
{
    auto maze = std::dynamic_pointer_cast<server_maze>(maze_);
    assert(maze != nullptr);
    return maze;
}

void server_world_manager::update_player_position(int player_id, int oldx, int oldy,
                                                  int newx, int newy)
{
    // no lags again
   assert( (newx - oldx == 0 ) || (newy - oldy == 0) );
   player_id_to_position[player_id] = std::make_pair(newx, newy);
}

messages::get_players_data_response server_world_manager::allocate_player_for_client()
{
    std::shared_ptr<server_player> player;
    for (auto &object : game_objects)
    {
        player = std::dynamic_pointer_cast<server_player>(object);
        if (player != nullptr)
        {
            if (!player->alive)
                break;
        }
    }
    // enaugh players number for everyone
    assert(player != nullptr);
    assert(player->alive == false);

    player->alive = true;
    auto pos = player->get_position();
    messages::get_players_data_response data = {player->id, std::get<0>(pos), std::get<1>(pos),
                                                player->alive};
    return data;
}

std::pair<int, int> server_world_manager::get_player_position(int player_id)
{
    return player_id_to_position[player_id];
}

}
