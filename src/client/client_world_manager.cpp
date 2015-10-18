#include "client_world_manager.hpp"
#include "../common/messages.hpp"

namespace core
{

client_world_manager::client_world_manager(std::shared_ptr<client_game_objects_factory> objects_factory_,
                                           std::shared_ptr<networking::client> client_)
    : abstract_world_manager(),
      objects_factory(objects_factory_),
      client(client_)
{
    logger_.log("client_world_manager: started");
}

networking::messages::get_enemies_data_response client_world_manager::get_enemies_data_from_network()
{
    networking::messages::get_enemies_data request;
    client->send_request(request);

    auto response = client->read_get_enemies_data_response();

    logger_.log("client_world_manager: get_enemies_data was load. Content dump:");
    size_t i = 0;
    for (; i < response.content.size(); i += 3)
    {
        if (i != 0 && (i % 15 == 0) )
            logger_.log_in_place("{%d, %d, %d}\n", response.content[i], response.content[i+1],
                response.content[i+2]);
        else
            logger_.log_in_place("{%d, %d, %d} ", response.content[i], response.content[i+1],
                response.content[i+2]);
    }
    if ((i-3)%15 != 0)
        logger_.log_in_place("\n");
    assert(response.content.size() % 3 == 0);
    return response;
}

networking::messages::get_players_data_response client_world_manager::get_players_data_from_network()
{
    networking::messages::get_players_data request;
    client->send_request(request);

    auto response = client->read_get_players_data_response();

    logger_.log("client_world_manager: get_players_data was load. Content dump:");
    size_t i = 0;
    for (; i < response.content.size(); i += 3)
    {
        if (i != 0 && (i % 15 == 0) )
            logger_.log_in_place("{%d, %d, %d}\n", response.content[i], response.content[i+1],
                response.content[i+2]);
        else
            logger_.log_in_place("{%d, %d, %d} ", response.content[i], response.content[i+1],
                response.content[i+2]);
    }
    if ((i-3)%15 != 0)
        logger_.log_in_place("\n");
    assert(response.content.size() % 3 == 0);

    return response;
}

int client_world_manager::get_id_data_from_network()
{
    networking::messages::get_id request;
    client->send_request(request);

    auto response = client->read_get_id_response();

    logger_.log("client_world_manager: player_id = %d", response.player_id);
    assert(response.player_id > 0);
    return response.player_id;
}

void client_world_manager::preprocess_loading()
{
    player_id = get_id_data_from_network();
    auto players_data = get_players_data_from_network();

    for (size_t i = 0; i < players_data.content.size(); i += 3)
    {
        int id = players_data.content[i];
        int x = players_data.content[i+1];
        int y = players_data.content[i+2];
        position_to_player_id[std::make_pair(x, y)] = id;

        if (id == player_id)
        {
            player_posx = x;
            player_posy = y;
        }
    }
    logger_.log("client_world_manager: build position_to_player_id map");

    auto enemies_data = get_enemies_data_from_network();

    for (size_t i = 0; i < enemies_data.content.size(); i += 3)
    {
        int id = enemies_data.content[i];
        int x = enemies_data.content[i+1];
        int y = enemies_data.content[i+2];
        position_to_enemy_id[std::make_pair(x, y)] = id;
    }
    logger_.log("client_world_manager: build position_to_enemy_id map");
}

void client_world_manager::postprocess_loading()
{
    int active_player_id = -1;
    for (size_t i = 0; i < game_objects.size(); i++)
    {
        auto object = game_objects[i];
        auto drawable_object = std::dynamic_pointer_cast<drawable>(object);
            if (drawable_object != nullptr)
                drawable_object->load_image();

       auto player = std::dynamic_pointer_cast<client_player>(object);
            if (player != nullptr)
            {
                if (player->is_active())
                    active_player_id = i;
            }
    }

    // I want active player to be on the end during ticking
    assert(active_player_id >= 0);
    std::swap(game_objects[active_player_id], game_objects.back());

    position_to_enemy_id.clear();
    position_to_player_id.clear();
}

void client_world_manager::preprocess_ticking()
{
    maze_->update_content();

    auto players_data = get_players_data_from_network();

    for (size_t i = 0; i < players_data.content.size(); i += 3)
    {
        int id = players_data.content[i];
        int x = players_data.content[i+1];
        int y = players_data.content[i+2];
        player_id_to_position[id] = std::make_pair(x, y);
    }
    // TO DO: I don't remove dead players yet...

    auto enemies_data = get_enemies_data_from_network();

    for (size_t i = 0; i < enemies_data.content.size(); i += 3)
    {
        int id = enemies_data.content[i];
        int x = enemies_data.content[i+1];
        int y = enemies_data.content[i+2];
        enemy_id_to_position[id] = std::make_pair(x, y);
    }
    logger_.log("client_world_manager: updated maze content and maps with positions");
}

void client_world_manager::make_maze(std::shared_ptr<maze_loader> loader)
{
    maze_ = objects_factory->create_client_maze(loader);
    logger_.log("client_world_manager: added maze");
}

void client_world_manager::add_enemy(int posx, int posy, int id)
{
    assert(maze_ != nullptr);
    game_objects.push_back(objects_factory->create_client_enemy(shared_from_this(), posx, posy, id));
    logger_.log("client_world_manager: added enemy on position = {%d, %d}", posx, posy);
}

void client_world_manager::shut_down_client()
{
    networking::messages::client_shutdown msg = {player_id};
    client->send_request(msg);
}

void client_world_manager::make_enemy(int posx, int posy)
{
    if (position_to_enemy_id.find(std::make_pair(posx, posy)) != position_to_enemy_id.end())
        add_enemy(posx, posy, position_to_enemy_id[std::make_pair(posx, posy)]);
    else
        assert(false);
}

void client_world_manager::make_player(int posx, int posy)
{
    assert(player_id > 0);
    assert(player_posx < INT_MAX);
    assert(player_posy < INT_MAX);

    auto player_pos = std::make_pair(posx, posy);
    assert(position_to_player_id.find(player_pos) != position_to_player_id.end());

    int id = position_to_player_id[player_pos];
    bool active = (id == player_id);

    game_objects.push_back(objects_factory->create_client_player(
                               shared_from_this(),
                               id,
                               posx,
                               posy,
                               active));
    logger_.log("client_world_manager: added player on position = {%d, %d}. Active = %b",
                posx, posy, active);
}

void client_world_manager::make_resource(const std::string &name, int posx, int posy)
{
    game_objects.push_back(objects_factory->create_client_resource(name, posx, posy));
    logger_.log("client_world_manager: added %s on position = {%d, %d}", name.c_str(), posx, posy);
}

bool client_world_manager::check_if_resource(std::shared_ptr<game_object> object)
{
    return std::dynamic_pointer_cast<client_resource>(object) != nullptr;
}

void client_world_manager::draw_all()
{
    assert(maze_ != nullptr);
    auto client_maze = std::dynamic_pointer_cast<core::client_maze>(maze_);
    assert(client_maze != nullptr);
    client_maze->draw();

    for (auto &object : game_objects)
        if (object != nullptr)
        {
            auto drawable_object = std::dynamic_pointer_cast<drawable>(object);
                if (drawable_object != nullptr)
                    drawable_object->draw();
        }
}

std::tuple<int, int> client_world_manager::get_enemy_position(int id)
{
    return enemy_id_to_position[id];
}

std::tuple<int, int> client_world_manager::get_player_position(int id)
{
    return player_id_to_position[id];
}

}


