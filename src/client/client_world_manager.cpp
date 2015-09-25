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

    logger_.log("client_world_manager: get_players_data was load. id = %d, posx = %d, posy = %d,"
                " active = %b", response.id, response.posx, response.posy, response.active);

    return response;
}

void client_world_manager::preprocess_loading()
{
    auto players_data = get_players_data_from_network();
    player_id = players_data.id;
    player_posx = players_data.posx;
    player_posy = players_data.posy;

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
    for (auto &object : game_objects)
    {
        auto drawable_object = std::dynamic_pointer_cast<drawable>(object);
            if (drawable_object != nullptr)
                drawable_object->load_image();
    }

    position_to_enemy_id.clear();
}

void client_world_manager::preprocess_ticking()
{
    maze_->update_content();

    // get_enemies_data
    // 1. get_enemies -> new_positions
    //    new_positions: id -> (x, y)
    // 2. enemy->tick updates position from new_positions

    auto enemies_data = get_enemies_data_from_network();

    for (size_t i = 0; i < enemies_data.content.size(); i += 3)
    {
        int id = enemies_data.content[i];
        int x = enemies_data.content[i+1];
        int y = enemies_data.content[i+2];
        enemy_id_to_position[id] = std::make_pair(x, y);
    }
    logger_.log("client_world_manager: updated maze content and enemy_id_to_position map");
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

    if (posx == player_posx && posy == player_posy)
    {
        game_objects.push_back(objects_factory->create_client_player(player_id, posx, posy, true));
        logger_.log("client_world_manager: added active player on position = {%d, %d}, id = %d",
                    posx, posy, player_id);
    }
    else
    {
        game_objects.push_back(objects_factory->create_client_player(0, posx, posy, false));
        logger_.log("client_world_manager: added unactive player on position = {%d, %d}",
                    posx, posy);
    }

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

}


