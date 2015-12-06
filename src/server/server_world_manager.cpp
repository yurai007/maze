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
            const char field = maze->get_field(column, row);
            if (field == 'P')
                make_player(column, row, false);
            else
                if (field == 'E')
                    make_enemy(column, row);
                else
                    if (field != 'X' && field != ' ')
                        make_resource(map_field_to_resource_name(field), column, row);
        }

    logger_.log("server_world_manager: all game objects were loaded successfully");
}

void server_world_manager::tick_all()
{
    static unsigned short tick_counter = 0;
    logger_.log("server_world_manager: started tick with id = %d", tick_counter);

    maze->tick(tick_counter);

    for (auto &player : players)
        tick_and_move(player, tick_counter);

    for (auto &enemy : enemies)
        tick_and_move(enemy, tick_counter);

    for (auto &resource : resources)
    {
        const auto position = resource->get_position();
        const char field = maze->get_field(std::get<0>(position), std::get<1>(position));

        if ( field != 'G' && field != 'M' && field != 'S'&& field != 'W' && field != 's')
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

std::vector<int> server_world_manager::get_enemies_data() const
{
    std::vector<int> result;
    for (auto &enemy : enemies)
    {
        const auto position = enemy->get_position();
        result.push_back(enemy->get_id());
        result.push_back(std::get<0>(position));
        result.push_back(std::get<1>(position));
    }
    return result;
}

std::vector<int> server_world_manager::get_players_data() const
{
    std::vector<int> players_data;

    for (const auto &player : players)
    {
        if (player->is_alive())
        {
            const auto position = player->get_position();
            players_data.push_back(player->get_id());
            players_data.push_back(std::get<0>(position));
            players_data.push_back(std::get<1>(position));
        }
    }
    return players_data;
}

std::pair<int, int> server_world_manager::get_player_position(int player_id) const
{
    const auto player_it = player_id_to_position.find(player_id);
    assert(player_it != player_id_to_position.end());
    return player_it->second;
}

std::shared_ptr<server_maze> server_world_manager::get_maze() const
{
    auto maze_to_return = std::dynamic_pointer_cast<server_maze>(maze);
    assert(maze_to_return != nullptr);
    return maze_to_return;
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
    assert(found_player != nullptr);
    const auto position = found_player->get_position();
    maze->reset_field(position);
}

void server_world_manager::update_player_position(int player_id, int oldx, int oldy,
                                                  int newx, int newy)
{
   assert( ((newx - oldx == 0 ) || (newy - oldy == 0) ) && ("Some lags happened") );
   player_id_to_position[player_id] = std::make_pair(newx, newy);
}

void server_world_manager::repair_if_uncorrect_enemies()
{
    for (const auto &enemy : enemies)
    {
        const auto position = enemy->get_position();
        const int posx = std::get<0>(position), posy = std::get<1>(position);
        const char field = maze->get_field(posx, posy);

        if (field != 'E')
        {
            logger_.log("server_world_manager: error! "
                        "Enemies cohesion verification failed for {%d, %d}",
                        posx, posy);
            maze->set_field(posx, posy, 'E');
        }
    }
}

void server_world_manager::repair_if_uncorrect_players()
{
    for (const auto &player : players)
    {
        if (player->is_alive())
        {
            const auto position = player->get_position();
            const int posx = std::get<0>(position), posy = std::get<1>(position);
            const char field = maze->get_field(posx, posy);

            if (field != 'P')
            {
                logger_.log("server_world_manager: error! "
                            "Players cohesion verification failed for {%d, %d}",
                            posx, posy);
                maze->set_field(posx, posy, 'P');
            }
        }
    }
}

std::string server_world_manager::map_field_to_resource_name(const char field) const
{
    const static std::unordered_map<char, std::string> field_to_resource_name =
    {
        {'G', "gold"}, {'M', "mercury"}, {'S', "stone"}, {'W', "wood"}, {'s', "sulfur"}
    };
    const auto name_it = field_to_resource_name.find(field);
    assert(name_it != field_to_resource_name.end());
    return name_it->second;
}

 // TO DO: why passsing by reference is not ok for shared_ptr - unknown conversion ??
void server_world_manager::tick_and_move(std::shared_ptr<game_object> some_game_object,
                                         unsigned short tick_counter)
{
    const auto old_position = some_game_object->get_position();
    some_game_object->tick(tick_counter);
    const auto new_position = some_game_object->get_position();

    if (new_position != old_position)
    {
        if (std::get<0>(new_position) < INT_MAX)
            maze->move_field(old_position, new_position);
        else
            maze->reset_field(old_position);
    }
}

void server_world_manager::load_maze_from_file()
{
    assert(maze != nullptr);
    maze->load();
}

void server_world_manager::make_enemy(int posx, int posy)
{
    assert(maze != nullptr);
    enemies.push_back(objects_factory->create_server_enemy(posx, posy));
    logger_.log("server_world_manager: added enemy on position = {%d, %d}", posx, posy);
}

std::shared_ptr<server_player> server_world_manager::make_player(int posx, int posy, bool alive)
{
    players.push_back(objects_factory->create_server_player(posx, posy, alive));
    logger_.log("server_world_manager: added player on position = {%d, %d}", posx, posy);
    return players.back();
}

void server_world_manager::make_resource(const std::string &name, int posx, int posy)
{
    resources.push_back(objects_factory->create_server_resource(name, posx, posy));
    logger_.log("server_world_manager: added %s on position = {%d, %d}", name.c_str(), posx, posy);
}

}
