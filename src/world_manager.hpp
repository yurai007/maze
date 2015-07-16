#ifndef WORLD_MANAGER_HPP
#define WORLD_MANAGER_HPP

#include <vector>
#include <memory>

#include "game_object.hpp"
#include "controller.hpp"
#include "maze.hpp"

namespace presentation
{
    class renderer;
}

namespace control
{
    class controller;
}

namespace core
{

class world_manager
{
public:
    world_manager(std::shared_ptr<presentation::renderer> renderer_,
                  std::shared_ptr<control::controller> controller_);
    void add_maze();
    void add_player();
    void load_all();
    void tick_all();
    void draw_all();

private:
    std::vector<std::shared_ptr<game_object>> game_objects;
    std::shared_ptr<maze> maze_;
    std::shared_ptr<presentation::renderer> renderer;
    std::shared_ptr<control::controller> controller;
};

}

#endif // WORLD_MANAGER_HPP
