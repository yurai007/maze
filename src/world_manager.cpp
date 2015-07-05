#include "world_manager.hpp"

namespace core
{

world_manager::world_manager()
{
}

void world_manager::load_all()
{

}

void world_manager::draw_all()
{
    for (auto &object : game_objects)
        object->draw();
}

}
