#include "client_world_manager.hpp"
#include "messages.hpp"

namespace core
{

client_world_manager::client_world_manager(std::shared_ptr<game_objects_factory> objects_factory_,
                                           std::shared_ptr<networking::client> client_)
    : abstract_world_manager(objects_factory_),
      client(client_)
{
    logger_.log("client_world_manager: started");
}

networking::messages::get_enemies_data_response client_world_manager::get_enemies_data_from_network()
{
    networking::messages::get_enemies_data request;
    client->send_request(request);

    auto response = client->read_get_enemies_data_response();
    assert(response.content.size() % 3 == 0);
    return response;
}

void client_world_manager::preprocess_loading()
{
    auto enemies_data = get_enemies_data_from_network();

    for (int i = 0; i < enemies_data.content.size(); i += 3)
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
        object->load();

    position_to_enemy_id.clear();
}

void client_world_manager::preprocess_ticking()
{
    maze_->update_content();

    // get_enemies_data
    // 1. get_enemies -> new_positions
    //    new_positions: id -> (x, y)
    // 2. enemy->tick updates position from new_positions

    // TO DO: I need second enemy type with different tick impl: server_enemy/client_enemy

    auto enemies_data = get_enemies_data_from_network();

    for (int i = 0; i < enemies_data.content.size(); i += 3)
    {
        int id = enemies_data.content[i];
        int x = enemies_data.content[i+1];
        int y = enemies_data.content[i+2];
        enemy_id_to_position[id] = std::make_pair(x, y);
    }
    logger_.log("client_world_manager: updated maze content and enemy_id_to_position map");
}

void client_world_manager::make_enemy(int posx, int posy)
{
    add_client_enemy(posx, posy, position_to_enemy_id[std::make_pair(posx, posy)]);
}

void client_world_manager::draw_all()
{
    assert(maze_ != nullptr);
    maze_->draw();

    for (auto &object : game_objects)
        if (object != nullptr)
            object->draw();
}

std::tuple<int, int> client_world_manager::get_enemy_position(int id)
{
    return enemy_id_to_position[id];
}

}


