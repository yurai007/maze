#ifndef WORLD_MANAGER_HPP
#define WORLD_MANAGER_HPP

#include <vector>
#include <memory>

#include "game_object.hpp"

namespace core
{

class world_manager
{
public:
    world_manager();
    void load_all();
    void draw_all();

private:
    std::vector<std::unique_ptr<game_object>> game_objects;
};

}

#endif // WORLD_MANAGER_HPP
