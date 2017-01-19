#include <cassert>
#include <climits>

#include "server_world_manager.hpp"
#include "../common/logger.hpp"

namespace core
{

server_world_manager::server_world_manager(smart::fit_smart_ptr<server_game_objects_factory>
                                           objects_factory_)
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
                make_player(column, row, false, last_player_id++);
            else
                if (field == 'E')
                    make_enemy(column, row, maze->get_id(column, row));
                else
                    if (field != 'X' && field != ' ')
                        make_resource(map_field_to_resource_name(field), column, row);
        }

    logger_.log("server_world_manager: all game objects were loaded successfully");
}

void server_world_manager::tick_all()
{
    static unsigned short tick_counter = 0;
    logger_.log_debug("server_world_manager: started tick with id = %d", tick_counter);

    maze->tick(tick_counter);

    for (auto &player : players)
        tick_and_move(player, tick_counter);

    for (auto &enemy : enemies)
        tick_and_move(enemy, tick_counter);

    // last resource is not nullptr
    int n = resources.size() - 1;
    while (n >= 0 && resources[n] == nullptr)
    {
        resources.pop_back();
        n--;
    }

    for (auto &resource : resources)
        if (resource != nullptr)
        {
            const auto position = resource->get_position();
            const char field = maze->get_field(std::get<0>(position), std::get<1>(position));

            if ( field != 'G' && field != 'M' && field != 'S'&& field != 'W' && field != 's')
            {
                resource = nullptr;
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

    if (resources.size() < 60)
        generate_resources(10);

    logger_.log_debug("server_world_manager: finished tick with id = %d", tick_counter);
    tick_counter++;
}

void server_world_manager::make_maze(smart::fit_smart_ptr<maze_loader> loader)
{
    maze = objects_factory->create_server_maze(loader);
    logger_.log("server_world_manager: added maze");
}

std::vector<int> server_world_manager::get_players_data() const
{
    // only for verification purpose
    std::unordered_map<int, std::pair<int, int>> players_map;

    for (int row = 0; row < maze->size(); row++)
        for (int column = 0; column < maze->size(); column++)
        {
            const char field = maze->get_field(column, row);
            if (field == 'P')
            {
                auto id =  maze->get_id(column, row);
                players_map[id] = {column, row};
            }
        }

    std::vector<int> players_data;
    for (const auto &player : players)
    {
        if (player->is_alive())
        {
            const auto position = player->get_position();
            players_data.push_back(player->get_id());
            players_data.push_back(std::get<0>(position));
            players_data.push_back(std::get<1>(position));

            auto id = player->get_id();

            if (std::get<0>(position) != players_map[id].first ||
                    std::get<1>(position) != players_map[id].second)
            {
                logger_.log_debug("A.{%d, %d, %d}", id, std::get<0>(position), std::get<1>(position));
                logger_.log_debug("B.{%d, %d, %d}", id, players_map[id].first, players_map[id].second);
            }
        }
    }

    return players_data;
}

std::vector<int> server_world_manager::get_resources_data() const
{
    std::vector<int> result;
    for (auto &resource : resources)
    {
        const auto position = resource->get_position();
        result.push_back(map_resource_name_to_type(resource->get_name()));
        result.push_back(std::get<0>(position));
        result.push_back(std::get<1>(position));
    }
    return result;
}

smart::fit_smart_ptr<server_maze> server_world_manager::get_maze() const
{
    assert(maze != nullptr);
    return maze;
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

    auto player = make_player(posx, posy, true, last_player_id++);
    assert(player != nullptr);

    maze->set_field(posx, posy, 'P', last_player_id-1);
    return player->get_id();
}

void server_world_manager::allocate_data_for_new_fireball(int player_id, int posx, int posy,
                                                         char direction)
{
    assert(direction == 'L' || direction == 'R' || direction == 'U' || direction == 'D');
    assert(maze->get_field(posx, posy) == 'P');

    make_fireball(player_id, posx, posy, direction);
    maze->set_field(posx, posy, direction);
}

void server_world_manager::generate_resources(unsigned resources_number)
{
    const int size = maze->size();
    const std::array<char, 5> resource_type = {'G', 'M', 'S', 'W', 's'};

    logger_.log("server_world_manager: new resources will be generated");

    for (unsigned i = 0; i < resources_number; i++)
    {
        unsigned n = rand()%5;
        unsigned posx = 0, posy = 0;
        while (maze->get_field(posx, posy) != ' ')
        {
            posx = rand()%size;
            posy = rand()%size;
        }
        make_resource(map_field_to_resource_name(resource_type[n]), posx, posy);
        maze->set_field(posx, posy, resource_type[n]);
    }
}

void server_world_manager::shutdown_player(int id)
{
    smart::fit_smart_ptr<server_player> found_player;
    for (auto &player : players)
    {
        if (player != nullptr)
        {
            if (player->get_id() == id)
            {
                found_player = player;
                player = nullptr;
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

void server_world_manager::update_player_position(
        int player_id, int oldx, int oldy,
        int newx, int newy)
{
   assert( ((newx - oldx == 0 ) || (newy - oldy == 0) ) && ("Some lags happened") );
   auto old_field = maze->get_field(oldx, oldy);
   assert(old_field == 'P');
   //auto new_field = maze->get_field(newx, newy);
   //assert(new_field != 'X' && new_field != 'E' && new_field != 'P');
   maze->move_field({oldx, oldy}, {newx, newy});

   for (auto &player : players)
       if (player->get_id() == player_id)
       {
            player->update_player_position(oldx, oldy, newx, newy);
            break;
       }
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

void server_world_manager::make_fireball(int player_id, int posx, int posy, char direction)
{
    fireballs.push_back(objects_factory->create_server_fireball(player_id, posx, posy, direction));
    logger_.log("server_world_manager: added fireball with direction = '%c'' on position = {%d, %d}",
                direction, posx, posy);
}

std::string server_world_manager::map_field_to_resource_name(const char field)
{
    static const std::unordered_map<char, std::string> field_to_resource_name =
    {
        {'G', "gold"}, {'M', "mercury"}, {'S', "stone"}, {'W', "wood"}, {'s', "sulfur"}
    };
    const auto name_it = field_to_resource_name.find(field);
    assert(name_it != field_to_resource_name.end());
    return name_it->second;
}

char server_world_manager::map_resource_name_to_type(const std::string &name)
{
    static const std::unordered_map<std::string, char> resource_name_to_field =
    {
        {"gold", 'G'}, {"mercury", 'M'}, {"stone", 'S'}, {"wood", 'W'}, {"sulfur", 's'}
    };
    const auto field_it = resource_name_to_field.find(name);
    assert(field_it != resource_name_to_field.end());
    return field_it->second;
}

 // TO DO: why passsing by reference is not ok for shared_ptr - unknown conversion ??
void server_world_manager::tick_and_move(smart::fit_smart_ptr<game_object> some_game_object,
                                         unsigned short tick_counter)
{
    assert(some_game_object != nullptr);
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

void server_world_manager::make_enemy(int posx, int posy, int id)
{
    assert(maze != nullptr);
    enemies.push_back(objects_factory->create_server_enemy(posx, posy, id));
    logger_.log("server_world_manager: added enemy on position = {%d, %d}", posx, posy);
}

smart::fit_smart_ptr<server_player> server_world_manager::make_player(int posx, int posy,
                                                                      bool alive, int id)
{
    players.push_back(objects_factory->create_server_player(posx, posy, alive, id));
    logger_.log("server_world_manager: added player on position = {%d, %d}", posx, posy);
    return players.back();
}

void server_world_manager::make_resource(const std::string &name, int posx, int posy)
{
    resources.push_back(objects_factory->create_server_resource(name, posx, posy));
    logger_.log("server_world_manager: added %s on position = {%d, %d}", name.c_str(), posx, posy);
}

}
