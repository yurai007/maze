#include <cassert>

#include "world_manager.hpp"
#include "player.hpp"
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
    game_objects.push_back(maze_);
}

void world_manager::add_player()
{
    assert(maze_ != nullptr);
    game_objects.push_back(std::make_shared<player>(renderer, controller, maze_));
}

void world_manager::load_all()
{
    for (auto &object : game_objects)
        object->load();
}

void world_manager::tick_all()
{
    for (auto &object : game_objects)
        object->tick();
}

void world_manager::draw_all()
{
    for (auto &object : game_objects)
        object->draw();
}

}
