#include <cassert>
#include <climits>

#include "server_world_manager.hpp"
#include "../common/logger.hpp"

namespace core
{

server_world_manager::server_world_manager(std::shared_ptr<server_game_objects_factory> objects_factory_)
    : objects_factory(objects_factory_)
{
    assert(objects_factory != nullptr);
    logger_.log("server_world_manager: started");
}

void server_world_manager::load_all()
{
    logger_.log("server_world_manager: start loading");

    load_maze_from_file();
    for (int row = 0; row < maze->size(); row++)
        for (int column = 0; column < maze->size(); column++)
        {
            char field = maze->get_field(column, row);
            if (field == 'P')
                make_player(column, row, false);
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

    logger_.log("server_world_manager: all game objects were loaded successfully");
}

void server_world_manager::tick_all()
{
    static unsigned short tick_counter = 0;
    logger_.log("server_world_manager: started tick with id = %d", tick_counter);

    maze->tick(tick_counter);

    for (auto &player : players)
    {
        const auto old_position = player->get_position();
        player->tick(tick_counter);
        const auto new_position = player->get_position();

        if (new_position != old_position)
        {
            if (std::get<0>(new_position) < INT_MAX)
                maze->move_field(old_position, new_position);
            else
                maze->reset_field(old_position);
        }
    }

    for (auto &enemy : enemies)
    {
        const auto old_position = enemy->get_position();
        enemy->tick(tick_counter);
        const auto new_position = enemy->get_position();

        if (new_position != old_position)
        {
            if (std::get<0>(new_position) < INT_MAX)
                maze->move_field(old_position, new_position);
            else
                maze->reset_field(old_position);
        }
    }

    for (auto &resource : resources)
    {
        const auto position = resource->get_position();
        const char field = maze->get_field(std::get<0>(position), std::get<1>(position));

        if ( field != 'G'&& field != 'M' && field != 'S'&& field != 'W' && field != 's')
        {
            resource.reset();
            logger_.log("server_world_manager: removed resource from positon = {%d, %d}",
                        std::get<0>(position), std::get<1>(position));
        }

    }

    for (auto &resource : resources)
        if (resource == nullptr)
        {
            std::swap(resource, resources.back());
            resources.pop_back();
        }

    logger_.log("server_world_manager: finished tick with id = %d", tick_counter);
    tick_counter++;
}

void server_world_manager::make_maze(std::shared_ptr<maze_loader> loader)
{
    maze = objects_factory->create_server_maze(loader);
    logger_.log("server_world_manager: added maze");
}

void server_world_manager::make_enemy(int posx, int posy)
{
    assert(maze != nullptr);
    enemies.push_back(objects_factory->create_server_enemy(posx, posy));
    logger_.log("server_world_manager: added enemy on position = {%d, %d}", posx, posy);
}

std::shared_ptr<server_player> server_world_manager::make_player(int posx, int posy, bool alive)
{
    auto player = objects_factory->create_server_player(posx, posy, alive);
    players.push_back(player);
    logger_.log("server_world_manager: added player on position = {%d, %d}", posx, posy);
    return player;
}

void server_world_manager::make_resource(const std::string &name, int posx, int posy)
{
    resources.push_back(objects_factory->create_server_resource(name, posx, posy));
    logger_.log("server_world_manager: added %s on position = {%d, %d}", name.c_str(), posx, posy);
}

std::vector<int> server_world_manager::get_enemies_data() const
{
    std::vector<int> result;
    for (auto &enemy : enemies)
    {
        auto position = enemy->get_position();
        const int posx = std::get<0>(position), posy = std::get<1>(position);
        const char field = maze->get_field(posx, posy);

        if (field != 'E')
        {
            logger_.log("server_world_manager: error! Enemies cohesion verification failed for {%d, %d}",
                        posx, posy);
            maze->set_field(posx, posy, 'E');
        }

        result.push_back(enemy->get_id());
        result.push_back(posx);
        result.push_back(posy);
    }
    return result;
}

std::shared_ptr<server_maze> server_world_manager::get_maze() const
{
    auto maze_to_return = std::dynamic_pointer_cast<server_maze>(maze);
    assert(maze_to_return != nullptr);
    return maze_to_return;
}

void server_world_manager::update_player_position(int player_id, int oldx, int oldy,
                                                  int newx, int newy)
{
    // no lags again
   assert( (newx - oldx == 0 ) || (newy - oldy == 0) );
   player_id_to_position[player_id] = std::make_pair(newx, newy);
}

std::vector<int> server_world_manager::get_players_data() const
{
    std::vector<int> players_data;

    for (const auto &player : players)
    {
        if (player->is_alive())
        {
            auto position = player->get_position();
            const int posx = std::get<0>(position), posy = std::get<1>(position);
            const char field = maze->get_field(posx, posy);

            if (field != 'P')
            {
                logger_.log("server_world_manager: error! Players cohesion verification failed for {%d, %d}",
                            posx, posy);
                maze->set_field(posx, posy, 'P');
            }

            players_data.push_back(player->get_id());
            players_data.push_back(posx);
            players_data.push_back(posy);
        }
    }
    return players_data;
}

int server_world_manager::allocate_data_for_new_player()
{
    const int size = maze->size();
    int posx = size/2, posy = size/2;

    while (maze->get_field(posx, posy) != ' ')
    {
        posx = rand()%size;
        posy = rand()%size;
    }

    auto player = make_player(posx, posy, true);
    assert(player != nullptr);

    maze->set_field(posx, posy, 'P');
    return player->get_id();
}

void server_world_manager::shutdown_player(int id)
{
    std::shared_ptr<server_player> found_player;
    for (auto &player : players)
    {
        if (player != nullptr)
        {
            if (player->get_id() == id)
            {
                found_player = player;
                player.reset();
                std::swap(player, players.back());
                players.pop_back();
                break;
            }
        }
    }
    // player really exists
    assert(found_player != nullptr);
    const auto position = found_player->get_position();
    maze->reset_field(position);
}

std::pair<int, int> server_world_manager::get_player_position(int player_id)
{
    assert(player_id_to_position.find(player_id) != player_id_to_position.end());
    return player_id_to_position[player_id];
}

void server_world_manager::load_maze_from_file()
{
    assert(maze != nullptr);
    maze->load();
}

}
