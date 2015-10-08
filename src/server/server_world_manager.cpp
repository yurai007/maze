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
    // I want to players on the beginning
    int current_offset_players = 0;
    for (size_t i = 0; i < game_objects.size(); i++)
    {
        auto player = std::dynamic_pointer_cast<server_player>(game_objects[i]);
        if (player != nullptr)
        {
            assert(current_offset_players < game_objects.size());
            std::swap(game_objects[current_offset_players], game_objects[i]);
            current_offset_players++;
        }
    }
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

std::vector<int> server_world_manager::get_players_data(bool verify) const
{
    std::vector<int> players_data;
    std::shared_ptr<server_player> player;

    for (auto &object : game_objects)
    {
        player = std::dynamic_pointer_cast<server_player>(object);
        if (player != nullptr)
        {
            auto position = player->get_position();
            if (verify)
            {
                char field = maze_->get_field(std::get<0>(position), std::get<1>(position));
                assert(field == 'P');
            }
            players_data.push_back(player->id);
            players_data.push_back(std::get<0>(position));
            players_data.push_back(std::get<1>(position));
        }
    }

    return players_data;
}

int server_world_manager::allocate_data_for_new_player()
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
    // player really exists
    assert(player != nullptr);
    assert(!player->alive);
    player->alive = true;
    return player->id;
}

void server_world_manager::shutdown_player(int id)
{
    std::shared_ptr<server_player> player;
    for (auto &object : game_objects)
    {
        player = std::dynamic_pointer_cast<server_player>(object);
        if (player != nullptr)
        {
            if (player->id == id)
                break;
        }
    }
    // player really exists
    assert(player != nullptr);
    assert(player->id == id);
    player->alive = false;
}

std::pair<int, int> server_world_manager::get_player_position(int player_id)
{
    return player_id_to_position[player_id];
}

}
