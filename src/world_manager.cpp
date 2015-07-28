#include <cassert>
#include <climits>

#include "world_manager.hpp"
#include "player.hpp"
#include "enemy.hpp"
#include "resource.hpp"
#include "renderer.hpp"

namespace core
{

world_manager::world_manager(
        std::shared_ptr<presentation::renderer> renderer_,
        std::shared_ptr<control::controller> controller_)
    : renderer(renderer_),
      controller(controller_)
{
}

void world_manager::add_maze()
{
    maze_ = std::make_shared<maze>(renderer);
}

void world_manager::add_player(int posx, int posy)
{
    assert(maze_ != nullptr);
    game_objects.emplace_back(std::make_shared<player>(renderer, controller, maze_, posx, posy));
}

void world_manager::add_enemy(int posx, int posy)
{
    assert(maze_ != nullptr);
    game_objects.emplace_back(std::make_shared<enemy>(renderer, maze_, posx, posy));
}

void world_manager::add_resource(const std::string &name, int posx, int posy)
{
    game_objects.emplace_back(std::make_shared<resource>(name, renderer, posx, posy));
}

void world_manager::load_all()
{
    maze_->load();

    for (int row = 0; row < maze_->size(); row++)
        for (int column = 0; column < maze_->size(); column++)
        {
            char field = maze_->get_field(column, row);
            if (field == 'P')
                add_player(column, row);
            else
                if (field == 'E')
                    add_enemy(column, row);
                else
                    if (field == 'G')
                        add_resource("gold", column, row);
        }

    for (auto &object : game_objects)
        object->load();
}

void world_manager::tick_all()
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
                    if (maze_->get_field(std::get<0>(old_position), std::get<1>(old_position)) != 'G')
                        object.reset();
            }
        }
    tick_counter++;
}

void world_manager::draw_all()
{
    maze_->draw();
    for (auto &object : game_objects)
        if (object != nullptr)
            object->draw();
}

}