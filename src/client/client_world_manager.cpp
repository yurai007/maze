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

smart::fit_smart_ptr<client_player> client_world_manager::find_player(int id)
{
    auto player_it = id_to_player.find(id);
    assert(player_it != id_to_player.end());
    return player_it->second;
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
    assert(maze != nullptr);
    logger_.log("client_world_manager%d: started tick with id = %d", player_id, tick_counter);

    assert(maze != nullptr);
    maze->update_content();

    auto players_data = get_players();

    logger_.log("client_world_manager%d: id_to_player.size() = %zu",
                player_id, id_to_player.size());

    for (auto &player_node : players_data)
    {
        auto id = player_node.first;
        auto x = player_node.second.first;
        auto y = player_node.second.second;

        if ((id_to_player.find(id) == id_to_player.end()) && (id != player_id))
        {
            auto new_player = make_external_player(id, x, y);
            if (!automatic_players)
            {
                if (new_player != nullptr)
                    new_player->load_image();
            }
        }
        else
            id_to_player[id]->new_tick(x, y);
    }

    remove_absent_player(players_data);
    update_enemies();

    auto resources_data = get_resources_data_from_network();
    for (size_t i = 0; i < resources_data.content.size(); i += 3)
    {
        char resource_type = (char)resources_data.content[i];
        int x = resources_data.content[i+1];
        int y = resources_data.content[i+2];

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
            auto player = find_player(player_node.first);
            if (player != nullptr)
                player->tick(tick_counter);
        }
    auto player = find_player(player_id);
    if (player != nullptr)
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
            auto player = find_player(player_node.first);
            if (player != nullptr)
                player->draw(player_x, player_y);
        }
    auto player = find_player(player_id);
    if (player != nullptr)
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
std::map<int, std::pair<int, int>> client_world_manager::get_players()
{
    logger_.log_debug("client_world_manager: players data from maze");
    std::map<int, std::pair<int, int>> tmp_player_id_to_position;

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

networking::messages::get_resources_data_response client_world_manager::get_resources_data_from_network()
{
    return network_manager->get_resources_data_from_network();
}

int client_world_manager::get_id_data_from_network()
{
    return network_manager->get_id_data_from_network();
}

// players_data must be sorted!
// id_to_player = id_to_player /
int client_world_manager::remove_absent_player(std::map<int, std::pair<int, int>> &players_data)
{
    auto player_it = players_data.begin();
    int removed_player_id = 0;

    auto item = id_to_player.begin();
    for (; (item != id_to_player.end()) && player_it != players_data.end(); )
    {
        auto id = item->first;
        if (player_it->first != id)
        {
            logger_.log("client_world_manager%d: removing player_id = %d from map",
                        player_id, id);
            removed_player_id = id;
            item = id_to_player.erase(item);
        }
        else
        {
            player_it++;
            item++;
        }
    }

    if (item != id_to_player.end())
    {
        logger_.log("client_world_manager%d: removing player_id = %d from map",
                    player_id, item->first);
        removed_player_id = item->first;
        id_to_player.erase(item);
    }
    return removed_player_id;
}

void client_world_manager::load_images_for_drawables()
{
    if (!automatic_players)
    {
        for (auto &player_node : id_to_player)
            if (player_node.first != player_id)
            {
                auto player = find_player(player_node.first);
                if (player != nullptr)
                    player->load_image();
            }
        auto player = find_player(player_id);
        if (player != nullptr)
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

smart::fit_smart_ptr<drawable> client_world_manager::make_external_player(int id, int posx, int posy)
{
    id_to_player[id] = objects_factory->create_client_player(
                               *this,
                               id,
                               posx,
                               posy,
                               false,
                               automatic_players);
    id_to_player[id]->new_tick(posx, posy);
    logger_.log("client_world_manager%d: added external player id = %d on position = {%d, %d}. "
                "Active = false, Automatic = %s",
                player_id, id, posx, posy, bool_to_string(automatic_players));
    return id_to_player[id];
}

void client_world_manager::make_resource(const std::string &name, int posx, int posy)
{
    resources_pos.insert({posx, posy});
    resources.push_back(objects_factory->create_client_resource(name, posx, posy));
    logger_.log("client_world_manager%d: added %s on position = {%d, %d}",
                player_id, name.c_str(), posx, posy);
}

}
