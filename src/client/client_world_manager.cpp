#include "client_world_manager.hpp"
#include "../common/messages.hpp"

namespace core
{

client_world_manager::client_world_manager(std::shared_ptr<client_game_objects_factory> objects_factory_,
                                           std::shared_ptr<networking::client> client_,
                                           bool automatic_players_)
    : abstract_world_manager(),
      objects_factory(objects_factory_),
      client(client_),
      automatic_players(automatic_players_)
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

void client_world_manager::load_image_if_not_automatic(std::shared_ptr<game_object> object)
{
    if (!automatic_players)
    {
        auto drawable_object = std::dynamic_pointer_cast<drawable>(object);
        if (drawable_object != nullptr)
            drawable_object->load_image();
    }
}

int client_world_manager::remove_absent_player(networking::messages::get_players_data_response &players_data)
{
    size_t current_pos = 0;
    int removed_player_id = 0;
    auto item = player_id_to_position.begin();
    for (; (item != player_id_to_position.end()) &&
         (current_pos <  players_data.content.size()); )
    {
        logger_.log("client_world_manager: item->key = %d,  current_pos = %d,  id = %d", item->first,
                    current_pos, players_data.content[current_pos]);

        if (players_data.content[current_pos] != item->first)
        {
            logger_.log("client_world_manager: removing player_id = %d from map", item->first);
            removed_player_id = item->first;
            item = player_id_to_position.erase(item);
        }
        else
        {
            item++;
            current_pos += 3;
        }
    }

    if (item != player_id_to_position.end())
    {
        logger_.log("client_world_manager: removing player_id = %d from map", item->first);
        removed_player_id = item->first;
        player_id_to_position.erase(item);
    }
    return removed_player_id;
}

void client_world_manager::postprocess_loading()
{
    int active_player_id = -1;
    for (size_t i = 0; i < game_objects.size(); i++)
    {
        auto object = game_objects[i];
        load_image_if_not_automatic(object);

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

    // I may assume that id-s are in increasing order
    // TO DO: I need order to iterate over keys. Key which is not touch should be removed
    for (size_t i = 0; i < players_data.content.size(); i += 3)
    {
        int id = players_data.content[i];
        int x = players_data.content[i+1];
        int y = players_data.content[i+2];
        if ((player_id_to_position.find(id) == player_id_to_position.end())
                && (id != player_id))
        {
            auto new_external_player = make_external_player(id, x, y);
            load_image_if_not_automatic(new_external_player);
        }
        player_id_to_position[id] = std::make_pair(x, y);
    }

    int removed_player_id = remove_absent_player(players_data);

    if (removed_player_id > 0)
    {
        for (auto &object : game_objects)
        {
            auto player = std::dynamic_pointer_cast<client_player>(object);
            if (player != nullptr)
            {
                if (player->id == removed_player_id)
                {
                    object.reset();
                    logger_.log("client_world_manager: removed player_id = %d from game_objects",
                                removed_player_id);
                    break;
                }
            }
        }
    }

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
    maze_ = objects_factory->create_client_maze(loader, !automatic_players);
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
                               active,
                               automatic_players));
    logger_.log("client_world_manager: added player on position = {%d, %d}. Active = %s, Automatic = %s",
                posx, posy, bool_to_string(active), bool_to_string(automatic_players));
}

std::shared_ptr<game_object> client_world_manager::make_external_player(int id, int posx, int posy)
{
    //auto player_pos = std::make_pair(posx, posy);
    //assert(position_to_player_id.find(player_pos) != position_to_player_id.end());

    //int id = position_to_player_id[player_pos];

    game_objects.push_back(objects_factory->create_client_player(
                               shared_from_this(),
                               id,
                               posx,
                               posy,
                               false,
                               automatic_players));
    logger_.log("client_world_manager: added external player on position = {%d, %d}. Active = false, Automatic = %s",
                posx, posy, bool_to_string(automatic_players));
    return game_objects.back();
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
    if (automatic_players)
        return;

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
    assert(player_id_to_position.find(id) != player_id_to_position.end());
    return player_id_to_position[id];
}

}


