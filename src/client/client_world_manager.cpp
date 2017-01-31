#include <typeinfo>

#include "client_world_manager.hpp"
#include "../common/messages.hpp"

#include "controller.hpp"
#include "renderer.hpp"

namespace core
{

client_world_manager::client_world_manager(smart::fit_smart_ptr<client_game_objects_factory> objects_factory_,
                                           smart::fit_smart_ptr<networking::network_manager> network_manager_,
                                           bool automatic_players_)
    : objects_factory(objects_factory_),
      network_manager(network_manager_),
      automatic_players(automatic_players_)
{
    assert(objects_factory != nullptr);
    assert(network_manager != nullptr);
    logger_.log("client_world_manager: started");
}

void client_world_manager::load_all()
{
    logger_.log("client_world_manager: start loading external players and enemies");
    player_id = get_id_data_from_network();

    logger_.log("client_world_manager: start loading maze");
    assert(maze != nullptr);
    maze->load();

    logger_.log("client_world_manager%d: build player_id_to_position map", player_id);

    for (int row = 0; row < maze->size(); row++)
        for (int column = 0; column < maze->size(); column++)
        {
            char field = maze->get_field(column, row);
            if (field == 'P')
                make_player(column, row);
            else
                if (field == 'E')
                    make_enemy(column, row);
                else
                    if (field == 'G')
                        make_resource("gold", column, row);
                    else
                        if (field == 'M')
                            make_resource("mercury", column, row);
                        else
                            if (field == 'S')
                                make_resource("stone", column, row);
                            else
                                if (field == 'W')
                                    make_resource("wood", column, row);
                                else
                                    if (field == 's')
                                        make_resource("sulfur", column, row);
        }

    load_images_for_drawables();
    logger_.log("client_world_manager%d: all game objects were loaded successfully", player_id);
}

void client_world_manager::tick_all()
{
    logger_.log("client_world_manager%d: started tick with id = %d", player_id, tick_counter);

    assert(maze != nullptr);
    maze->update_content();

        auto players_data = get_players();

        // pass one - add new
        for (auto &player : players_data)
        {
            auto id = player.first;

            if (id_to_player.find(id) == id_to_player.end())
            {
                int posx = std::get<0>(player.second), posy = std::get<1>(player.second);
                id_to_player[id] = objects_factory->create_client_player(*this, id, posx, posy,
                                  false, automatic_players);

                logger_.log("client_world_manager%d: added external player id = %d on position = {%d, %d}. "
                            "Active = false, Automatic = %s",
                            player_id, id, posx, posy, bool_to_string(automatic_players));
                auto drawable_player = id_to_player[id];

                if (!automatic_players)
                    drawable_player->load_image();
            }
        }

        // pass two - remove old and new_tick
        auto itr = id_to_player.begin();
        while (itr != id_to_player.end())
        {
            auto id = itr->first;
            auto player = players_data.find(id);
            if (player == players_data.end())
            {
                itr = id_to_player.erase(itr);
                logger_.log("client_world_manager%d: removing player_id = %d from map",
                                        player_id, id);
            }
            else
            {
                int posx = std::get<0>(player->second), posy = std::get<1>(player->second);
                id_to_player[id]->new_tick(posx, posy);
                ++itr;
            }
        }

    update_enemies();

    auto resources_data = get_resources();
    for (size_t i = 0; i < resources_data.size(); i += 3)
    {
        char resource_type = (char)resources_data[i];
        int x = resources_data[i+1];
        int y = resources_data[i+2];

        if (resources_pos.find({x, y}) == resources_pos.end())
        {
             make_resource(map_field_to_resource_name(resource_type), x, y);
             char field = maze->get_field(x, y);
             if (field == ' ')
             {
                 // problem: get_chunk_response and get_resource_response NOT in-sync
                 logger_.log("client_world_manager%d: resources NOT in-sync for {%d, %d}", player_id,
                             x, y);
                 maze->set_field(x, y, resource_type);
                 field = resource_type;
             }
        }
    }
    logger_.log("client_world_manager%d: updated maze content and maps with positions", player_id);

    maze->tick(tick_counter);

    for (auto &resource : resources)
        if (resource != nullptr)
        {
            resource->tick(tick_counter);
            const auto position = resource->get_position();
            const int x = std::get<0>(position);
            const int y = std::get<1>(position);
            const char field = maze->get_field(x, y);

            if ( field != 'G' && field != 'M' && field != 'S' && field != 'W' && field != 's')
            {
                resource = nullptr;
                resources_pos.erase({x, y});
                logger_.log("client_world_manager: removed resource from positon = {%d, %d}",
                            x, y);
            }
        }

    // players on the end
    for (auto &player_node : id_to_player)
        if (player_node.first != player_id)
        {
            assert(player_node.second != nullptr);
            player_node.second->tick(player_id);
        }
    auto &player = id_to_player[player_id];
    assert(player != nullptr);
    player->tick(tick_counter);

    logger_.log("client_world_manager%d: finished tick with id = %d", player_id, tick_counter);
    tick_counter++;
}

void client_world_manager::draw_all()
{
    if (automatic_players)
        return;

    assert(maze != nullptr);

    int player_x, player_y;
    auto &active_player = id_to_player[player_id];
    std::tie(player_x, player_y) = active_player->get_position();

    maze->draw(player_x, player_y);

    for (auto &enemy : id_to_enemy)
        if (enemy.second != nullptr)
            enemy.second->draw(player_x, player_y);

    for (auto &resource : resources)
        if (resource != nullptr)
            resource->draw(player_x, player_y);


    for (auto &player_node : id_to_player)
        if (player_node.first != player_id)
        {
            assert(player_node.second != nullptr);
            player_node.second->draw(player_x, player_y);
        }
    auto &player = id_to_player[player_id];
    assert(player != nullptr);
    player->draw(player_x, player_y);

}

std::string client_world_manager::map_field_to_resource_name(const char field)
{
    static const std::unordered_map<char, std::string> field_to_resource_name =
    {
        {'G', "gold"}, {'M', "mercury"}, {'S', "stone"}, {'W', "wood"}, {'s', "sulfur"}
    };
    const auto name_it = field_to_resource_name.find(field);
    assert(name_it != field_to_resource_name.end());
    return name_it->second;
}

void client_world_manager::update_enemies()
{
    logger_.log_debug("client_world_manager: update enemies data from maze");

    for (int row = 0; row < maze->size(); row++)
        for (int column = 0; column < maze->size(); column++)
        {
            const char field = maze->get_field(column, row);
            if (field == 'E')
            {
                auto id =  maze->get_id(column, row);
                auto &enemy = id_to_enemy[id];
                enemy->new_tick(column, row);
                //logger_.log_debug("{%d, %d, %d}", id, column, row);
            }
        }
}

// must be sorted by id!
std::map<int, std::tuple<int, int>> client_world_manager::get_players()
{
    logger_.log_debug("client_world_manager: players data from maze");
    std::map<int, std::tuple<int, int>> tmp_player_id_to_position;

    for (int row = 0; row < maze->size(); row++)
        for (int column = 0; column < maze->size(); column++)
        {
            const char field = maze->get_field(column, row);
            if (field == 'P')
            {
                auto id =  maze->get_id(column, row);
                tmp_player_id_to_position[id] = {column, row};
                //logger_.log_debug("{%d, %d, %d}", id, column, row);
            }
        }
    return tmp_player_id_to_position;
}

std::vector<int> client_world_manager::get_resources()
{
    logger_.log_debug("client_world_manager: resource data from maze");
    std::map<std::tuple<int, int>, char> tmp_resource_id_to_position;

    for (int row = 0; row < maze->size(); row++)
        for (int column = 0; column < maze->size(); column++)
        {
            const char field = maze->get_field(column, row);
            if ( field == 'G' || field == 'M' || field == 'S' || field == 'W' || field == 's')
            {
                //auto id =  maze->get_id(column, row);
                std::tuple<int, int> pos = {column, row};
                tmp_resource_id_to_position[pos] = field;
                //logger_.log_debug("{%c, %d, %d}", field, column, row);
            }
        }

    std::vector<int> msg;
        for (auto &it : tmp_resource_id_to_position)
        {
            int x = std::get<0>(it.first);
            int y = std::get<1>(it.first);
            char resource_type = it.second;
            msg.push_back((int)resource_type);
            msg.push_back(x);
            msg.push_back(y);
        }
    return msg;
}

int client_world_manager::get_id_data_from_network()
{
    return network_manager->get_id_data_from_network();
}

void client_world_manager::load_images_for_drawables()
{
    if (!automatic_players)
    {
        for (auto &player_node : id_to_player)
            if (player_node.first != player_id)
            {
                assert(player_node.second != nullptr);
                player_node.second->load_image();
            }
        auto &player = id_to_player[player_id];
        assert(player != nullptr);
        player->load_image();
    }

    for (auto &enemy_node : id_to_enemy)
    {
        auto &enemy = enemy_node.second;
        if (!automatic_players)
        {
            if (enemy != nullptr)
                enemy->load_image();
        }
    }

    for (auto &resource : resources)
    {
        if (!automatic_players)
        {
            if (resource != nullptr)
                resource->load_image();
        }
    }
}

void client_world_manager::make_maze(smart::fit_smart_ptr<maze_loader> loader)
{
    maze = objects_factory->create_client_maze(loader, !automatic_players);
    logger_.log("client_world_manager%d: added maze", player_id);
}

void client_world_manager::add_enemy(int posx, int posy, int id)
{
    assert(maze != nullptr);
    assert(id_to_enemy.find(id) == id_to_enemy.end());
    id_to_enemy[id] = objects_factory->create_client_enemy(posx, posy, id);
    logger_.log("client_world_manager%d: added enemy on position = {%d, %d}", player_id, posx, posy);
}

void client_world_manager::shut_down_client()
{
    network_manager->shut_down_client(player_id);
}

void client_world_manager::make_enemy(int posx, int posy)
{
    auto id = maze->get_id(posx, posy);
    add_enemy(posx, posy, id);
}

void client_world_manager::make_player(int posx, int posy)
{
    assert(player_id >= 0);
    assert(maze->get_field(posx, posy) == 'P');

    int id = maze->get_id(posx, posy);
    bool active = (id == player_id);

    id_to_player[id] = objects_factory->create_client_player(
                               *this,
                               id,
                               posx,
                               posy,
                               active,
                               automatic_players);
    id_to_player[id]->new_tick(posx, posy);

    if (active)
    {
        assert(maze != nullptr);
        maze->attach_active_player(id_to_player[id]);
    }

    logger_.log("client_world_manager%d: added player on position = {%d, %d}. Active = %s, Automatic = %s",
                player_id, posx, posy, bool_to_string(active), bool_to_string(automatic_players));
}

void client_world_manager::make_resource(const std::string &name, int posx, int posy)
{
    resources_pos.insert({posx, posy});
    resources.push_back(objects_factory->create_client_resource(name, posx, posy));
    logger_.log("client_world_manager%d: added %s on position = {%d, %d}",
                player_id, name.c_str(), posx, posy);
}

}
