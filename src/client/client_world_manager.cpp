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
                if ( field == 'G' || field == 'M' || field == 'S' || field == 'W' || field == 's')
                    make_resource(field, column, row);
        }

    if (!automatic_players)
        load_images_for_drawables();
    logger_.log("client_world_manager%d: all game objects were loaded successfully", player_id);
}

void client_world_manager::tick_all()
{
    logger_.log("client_world_manager%d: started tick with id = %d", player_id, tick_counter);

    assert(maze != nullptr);
    maze->update_content();

    update_players();
    // TO DO: maybe player should be updated on the end?
    // TO DO: some fuckup with resources number - dump all resources on both sides!

    update_enemies();
    update_resources();

    maze->tick(tick_counter);

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

    if (player->died)
    {
        shut_down_client();
        killed = true;
    }

    logger_.log("client_world_manager%d: resources number: %u", player_id, resources.size());
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
        if (resource.second != nullptr)
            resource.second->draw(player_x, player_y);

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
            }
        }
}

// must be sorted by id!
void client_world_manager::update_players()
{
    logger_.log_debug("client_world_manager: update players data from maze");

    // pass one - add new
    std::map<int, std::tuple<int, int>> id_to_position_maze;
    for (int row = 0; row < maze->size(); row++)
        for (int column = 0; column < maze->size(); column++)
        {
            const char field = maze->get_field(column, row);
            if (field == 'P')
            {
                auto id =  maze->get_id(column, row);
                id_to_position_maze[id] = {column, row};

                if (id_to_player.find(id) == id_to_player.end())
                {
                    int posx = column, posy = row;
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
        }

    // pass two - remove old and new_tick
    auto itr = id_to_player.begin();
    while (itr != id_to_player.end())
    {
        auto id = itr->first;
        auto player = id_to_position_maze.find(id);
        if (player == id_to_position_maze.end())
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
}

void client_world_manager::update_resources()
{
    logger_.log_debug("client_world_manager: update resource data from maze");

    // pass one - add new
    for (int row = 0; row < maze->size(); row++)
        for (int column = 0; column < maze->size(); column++)
        {
            const char field = maze->get_field(column, row);
            if ( field == 'G' || field == 'M' || field == 'S' || field == 'W' || field == 's')
            {
                auto it = resources.find({column, row});
                if (it == resources.end())
                    make_resource(field, column, row);
            }
        }
    // pass two - remove old
    int x, y;
    auto itr = resources.begin();
    while (itr != resources.end())
    {
        auto resource = (*itr).second;
        const auto pos = resource->get_position();
        std::tie(x, y) = pos;
        const char field = maze->get_field(x, y);
        if (!( field == 'G' || field == 'M' || field == 'S' || field == 'W' || field == 's'))
        {
            itr = resources.erase(itr);
            logger_.log("client_world_manager: removed resource from positon = {%d, %d}",
                        x, y);
        }
        else
        {
            ++itr;
        }
    }
}

int client_world_manager::get_id_data_from_network()
{
    return network_manager->get_id_data_from_network();
}

void client_world_manager::load_images_for_drawables()
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

    for (auto &enemy_node : id_to_enemy)
    {
        if (enemy_node.second != nullptr)
            enemy_node.second->load_image();
    }

    for (auto &resource : resources)
    {
        if (resource.second != nullptr)
            resource.second->load_image();
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

    id_to_player[id] = objects_factory->create_client_player(*this, id, posx, posy, active,
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

void client_world_manager::make_resource(const char field, int posx, int posy)
{
    static const std::unordered_map<char, std::string> field_to_resource_name =
    {
        {'G', "gold"}, {'M', "mercury"}, {'S', "stone"}, {'W', "wood"}, {'s', "sulfur"}
    };
    const auto name_it = field_to_resource_name.find(field);
    assert(name_it != field_to_resource_name.end());
    auto name = name_it->second;
    auto pos = std::tie(posx, posy);
    resources[pos] = objects_factory->create_client_resource(name, posx, posy);

    logger_.log("client_world_manager%d: added %s on position = {%d, %d}",
                player_id, name.c_str(), posx, posy);
}

}
