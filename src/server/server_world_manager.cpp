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

void server_world_manager::load_all(smart::fit_smart_ptr<maze_loader> loader)
{
    logger_.log("server_world_manager: start loading");    

    maze = objects_factory->create_server_maze(loader);
    maze->load();
    logger_.log("server_world_manager: added maze");

    for (int row = 0; row < maze->size(); row++)
        for (int column = 0; column < maze->size(); column++)
        {
            const char field = maze->get_field(column, row);
            if (field == 'P')
            {
                players.push_back(objects_factory->create_server_player(column, row, false, last_player_id++));
                logger_.log("server_world_manager: added player on position = {%d, %d}", column, row);
            }
            else
                if (field == 'E')
                {
                    auto id = maze->get_id(column, row);
                    enemies.push_back(objects_factory->create_server_enemy(column, row, id));
                    logger_.log("server_world_manager: added enemy on position = {%d, %d}", column, row);
                }
                else
                    if (field != 'X' && field != ' ')
                    {
                        resources_number++;
                        logger_.log("server_world_manager: added %c on position = {%d, %d}",
                                    field, column, row);
                    }
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

    if (resources_number < 60)
        generate_resources(10);

    for (auto &fireball : fireballs)
    {
        const auto old_position = fireball->get_position();
        fireball->tick(tick_counter);
        const auto new_position = fireball->get_position();

        if (new_position != old_position)
        {
            if (std::get<0>(new_position) < INT_MAX)
            {
                int posx = std::get<0>(new_position), posy = std::get<1>(new_position);
                // TO DO: because of is_field_filled impl now resources are blown up by
                //        fireball -> should be removed
                if (maze->in_range(posx, posy) && !maze->is_field_filled(posx, posy))
                {
                    int oldx = std::get<0>(old_position), oldy = std::get<1>(old_position);
                    maze->move_field(old_position, new_position);
                    logger_.log("server_world_manager: fireball move: {%d,%d} -> {%d,%d}", oldx, oldy,
                                posx, posy);
                }
                else
                {
                    fireball->freeze = true;
                    logger_.log("server_world_manager: fireball freeze on {%d,%d}", posx, posy);
                }
            }
        }
    }

    logger_.log_debug("server_world_manager: finished tick with id = %d", tick_counter);
    logger_.log("server_world_manager: resources number: %u", resources_number);
    tick_counter++;
}

smart::fit_smart_ptr<server_maze> server_world_manager::get_maze() const
{
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

    players.push_back(objects_factory->create_server_player(posx, posy, true, last_player_id++));
    maze->set_field(posx, posy, 'P', last_player_id-1);
    logger_.log("server_world_manager: added player on position = {%d, %d}", posx, posy);
    return players.back()->get_id();
}

bool server_world_manager::allocate_new_fireball_if_possible(int player_id, int posx, int posy,
                                                         char direction)
{
    assert(direction == 'L' || direction == 'R' || direction == 'U' || direction == 'D');
    if (maze->get_field(posx, posy) != ' ')
        return false;

    fireballs.push_back(objects_factory->create_server_fireball(player_id, posx, posy, direction));
    last_fireball_id++;

    maze->set_field(posx, posy, 'F', last_fireball_id-1);
    logger_.log("server_world_manager: added fireball: id = %d, direction = '%c'', position = {%d, %d}",
                last_fireball_id-1, direction, posx, posy);
    return true;
}

void server_world_manager::generate_resources(unsigned resources)
{
    const int size = maze->size();
    const std::array<char, 5> resource_type = {'G', 'M', 'S', 'W', 's'};

    logger_.log("server_world_manager: new resources will be generated");

    for (unsigned i = 0; i < resources; i++)
    {
        unsigned n = rand()%5;
        unsigned posx = 0, posy = 0;
        while (maze->get_field(posx, posy) != ' ')
        {
            posx = rand()%size;
            posy = rand()%size;
        }

        resources_number++;
        maze->set_field(posx, posy, resource_type[n]);
        logger_.log("server_world_manager: added %c on position = {%d, %d}", resource_type[n], posx, posy);
    }

    logger_.log("server_world_manager: resources number: %u", resources_number);
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
            player->update_player_position(newx, newy);
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

void server_world_manager::remove_resource(auto pos)
{
    auto field = maze->get_field(std::get<0>(pos), std::get<1>(pos));
    if (field == 'G' || field == 'M' || field == 'S'|| field == 'W' || field == 's')
    {
        resources_number--;
        logger_.log("server_world_manager: removed resource from positon = {%d, %d}",
                    std::get<0>(pos), std::get<1>(pos));
    }
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
        {
            remove_resource(new_position);
            maze->move_field(old_position, new_position);
        }
        else
        {
            remove_resource(old_position);
            maze->reset_field(old_position);
        }
    }
}

}
