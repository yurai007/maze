#include <cassert>
#include <climits>

#include "server_world_manager.hpp"
#include "remote_player.hpp"
#include "enemy.hpp"
#include "resource.hpp"
#include "renderer.hpp"


namespace core
{

server_world_manager::server_world_manager(
        std::shared_ptr<presentation::renderer> renderer_,
        std::shared_ptr<control::controller> controller_,
        std::shared_ptr<networking::client> client_)
    : renderer(renderer_),
      controller(controller_),
      client(client_)
{
    logger_.log("server_world_manager: started");
}

void server_world_manager::add_maze(std::shared_ptr<maze_loader> loader)
{
    maze_ = std::make_shared<maze>(renderer, loader);
    logger_.log("server_world_manager: added maze");
}

void server_world_manager::add_remote_player(int posx, int posy)
{
    assert(maze_ != nullptr);
    game_objects.push_back(std::make_shared<remote_player>(renderer, controller, maze_, client, posx, posy));
    logger_.log("server_world_manager: added player on position = {%d, %d}", posx, posy);
}

void server_world_manager::add_enemy(int posx, int posy)
{
    assert(maze_ != nullptr); 
    game_objects.push_back(std::make_shared<enemy>(renderer, maze_, posx, posy)); //Wtf ?? emplace_back
    logger_.log("server_world_manager: added enemy on position = {%d, %d}", posx, posy);
}

void server_world_manager::add_resource(const std::string &name, int posx, int posy)
{
    game_objects.push_back(std::make_shared<resource>(name, renderer, posx, posy));
    logger_.log("server_world_manager: added %s on position = {%d, %d}", name.c_str(), posx, posy);
}

void server_world_manager::load_all()
{
    maze_->load();

    // On client I need extra phase here to obtain id-s

    for (int row = 0; row < maze_->size(); row++)
        for (int column = 0; column < maze_->size(); column++)
        {
            char field = maze_->get_field(column, row);
            if (field == 'P')
                add_remote_player(column, row);
            else
                if (field == 'E')
                    add_enemy(column, row);
                else
                    if (field == 'G')
                        add_resource("gold", column, row);
        }

    if (renderer == nullptr)
        return;

    for (auto &object : game_objects)
        object->load();

    logger_.log("server_world_manager: all game objects were loaded successfully");
}

void server_world_manager::tick_all()
{
    static unsigned short tick_counter = 0;

    maze_->tick(tick_counter);
    for (auto &object : game_objects)
        if (object != nullptr)
        {
            auto old_position = object->get_position();
            object->tick(tick_counter);
            auto new_position = object->get_position();

            if (new_position != old_position)
            {
                int new_pos_x = std::get<0>(new_position);
                int new_pos_y = std::get<1>(new_position);

                int old_pos_x = std::get<0>(old_position);
                int old_pos_y = std::get<1>(old_position);

                if (new_pos_x < INT_MAX)
                    maze_->move_field(old_pos_x, old_pos_y, new_pos_x, new_pos_y);
                else
                    maze_->reset_field(old_pos_x, old_pos_y);
            }
            else
            {
                // dirty hack, downcasting for zombie
                if (std::dynamic_pointer_cast<resource>(object) != nullptr)
                    // for resource client: get_chunk
                    if (maze_->get_field(std::get<0>(old_position), std::get<1>(old_position)) != 'G')
                    {
                        object.reset();
                        logger_.log("server_world_manager: removed resource from positon = {%d, %d}",
                            std::get<0>(old_position), std::get<1>(old_position));
                    }
            }
    }
    if (tick_counter%10 == 0)
        logger_.log("server_world_manager: finished tick with id = %d successfully", tick_counter);
    tick_counter++;
}

void server_world_manager::tick_all_client()
{
    assert(client != nullptr);

    /* for small number of enemies:
            client: get_enemy(id)_pos??
       otherwise:
            client: get_chunk for all visible chunks
    */
}

void server_world_manager::draw_all()
{
    if (renderer == nullptr)
        return;

    maze_->draw();
    for (auto &object : game_objects)
        if (object != nullptr)
            object->draw();
}

}
