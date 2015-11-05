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
                make_player(column, row);
            else
                if (field == 'E')
                    make_enemy(column, row);
                else
                    if (field == 'G')
                        make_resource("gold", column, row);
        }

    logger_.log("server_world_manager: all game objects were loaded successfully");
}

void server_world_manager::tick_all()
{
    static unsigned short tick_counter = 0;
    logger_.log("server_world_manager: started tick with id = %d", tick_counter);

    maze->tick(tick_counter);

    for (auto &object : players_and_resources)
    {
        auto player = std::dynamic_pointer_cast<server_player>(object);
        if (player != nullptr)
        {
            auto old_position = player->get_position();
            player->tick(tick_counter);
            auto new_position = player->get_position();

            if (new_position != old_position)
            {
                int new_pos_x = std::get<0>(new_position);
                int new_pos_y = std::get<1>(new_position);

                int old_pos_x = std::get<0>(old_position);
                int old_pos_y = std::get<1>(old_position);

                if (new_pos_x < INT_MAX)
                    maze->move_field(old_pos_x, old_pos_y, new_pos_x, new_pos_y);
                else
                    maze->reset_field(old_pos_x, old_pos_y);
            }
        }
    }

    for (auto &enemy : enemies)
    {
        auto old_position = enemy->get_position();
        enemy->tick(tick_counter);
        auto new_position = enemy->get_position();

        if (new_position != old_position)
        {
            int new_pos_x = std::get<0>(new_position);
            int new_pos_y = std::get<1>(new_position);

            int old_pos_x = std::get<0>(old_position);
            int old_pos_y = std::get<1>(old_position);

            if (new_pos_x < INT_MAX)
                maze->move_field(old_pos_x, old_pos_y, new_pos_x, new_pos_y);
            else
                maze->reset_field(old_pos_x, old_pos_y);
        }
    }

    for (auto &object : players_and_resources)
    {
        auto resource = std::dynamic_pointer_cast<server_resource>(object);
        if (resource != nullptr)
        {
            auto position = resource->get_position();

            if (maze->get_field(std::get<0>(position), std::get<1>(position)) != 'G')
            {
                object.reset();
                logger_.log("server_world_manager: removed resource from positon = {%d, %d}",
                    std::get<0>(position), std::get<1>(position));
            }
        }
    }

    logger_.log("server_world_manager: finished tick with id = %d", tick_counter);
    tick_counter++;
}

//void server_world_manager::move_players_on_beginning()
//{
//    int current_offset_players = 0;
//    for (size_t i = 0; i < players_and_resources.size(); i++)
//    {
//        auto player = std::dynamic_pointer_cast<server_player>(players_and_resources[i]);
//        if (player != nullptr)
//        {
//            assert(current_offset_players < players_and_resources.size());
//            std::swap(players_and_resources[current_offset_players], players_and_resources[i]);
//            current_offset_players++;
//        }
//    }
//}

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

std::shared_ptr<server_player> server_world_manager::make_player(int posx, int posy)
{
    auto player = objects_factory->create_server_player(posx, posy);
    players_and_resources.push_back(player);
    logger_.log("server_world_manager: added player on position = {%d, %d}", posx, posy);
    return player;
}

void server_world_manager::make_resource(const std::string &name, int posx, int posy)
{
    players_and_resources.push_back(objects_factory->create_server_resource(name, posx, posy));
    logger_.log("server_world_manager: added %s on position = {%d, %d}", name.c_str(), posx, posy);
}

std::vector<int> server_world_manager::get_enemies_data(bool verify) const
{
    std::vector<int> result;
    for (auto &enemy : enemies)
    {
        auto position = enemy->get_position();

        if (verify)
        {
            char field = maze->get_field(std::get<0>(position), std::get<1>(position));
            assert(field == 'E');
        }

        result.push_back(enemy->get_id());
        result.push_back(std::get<0>(position));
        result.push_back(std::get<1>(position));
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

std::vector<int> server_world_manager::get_players_data(bool verify) const
{
    std::vector<int> players_data;
    std::shared_ptr<server_player> player;

    for (auto &object : players_and_resources)
    {
        player = std::dynamic_pointer_cast<server_player>(object);
        if (player != nullptr && player->alive)
        {
            auto position = player->get_position();
            if (verify)
            {
                char field = maze->get_field(std::get<0>(position), std::get<1>(position));
                assert(field == 'P');
            }
            players_data.push_back(player->id);
            players_data.push_back(std::get<0>(position));
            players_data.push_back(std::get<1>(position));
        }
    }

    return players_data;
}

int server_world_manager::allocate_data_for_new_player()
{
    int posx = maze->size()/2;
    int posy = maze->size()/2;

    while (maze->get_field(posx, posy) != ' ')
    {
        posx = rand()%20;
        posy = rand()%20;
    }

    auto player = make_player(posx, posy);
    assert(player != nullptr);
    player->alive = true;
    maze->set_field(posx, posy, 'P');

    return player->id;
}

void server_world_manager::shutdown_player(int id)
{
    std::shared_ptr<server_player> player;
    for (auto &object : players_and_resources)
    {
        player = std::dynamic_pointer_cast<server_player>(object);
        if (player != nullptr)
        {
            if (player->id == id)
            {
                object.reset();
                break;
            }
        }
    }
    // player really exists
    assert(player != nullptr);
    auto position = player->get_position();
    maze->reset_field(std::get<0>(position), std::get<1>(position));
    //player.reset();
}

std::pair<int, int> server_world_manager::get_player_position(int player_id)
{
    return player_id_to_position[player_id];
}

void server_world_manager::load_maze_from_file()
{
    assert(maze != nullptr);
    maze->load();
}

}
