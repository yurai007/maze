#ifndef ABSTRACT_WORLD_MANAGER_HPP
#define ABSTRACT_WORLD_MANAGER_HPP

#include <vector>
#include <memory>

#include "game_object.hpp"
#include "maze_loader.hpp"
#include "abstract_maze.hpp"

namespace core
{

class abstract_world_manager
{
public:
    abstract_world_manager() = default;

    virtual void preprocess_loading() = 0;
    virtual void postprocess_loading() = 0;
    virtual void preprocess_ticking() = 0;
    virtual void make_maze(std::shared_ptr<maze_loader> loader) = 0;
    virtual void make_enemy(int posx, int posy) = 0;
    virtual void make_player(int posx, int posy) = 0;
    virtual void make_resource(const std::string &name, int posx, int posy) = 0;
    virtual bool check_if_resource(std::shared_ptr<game_object> object) = 0;

    void tick_all(bool omit_moving_fields);
    void load_all();
    virtual ~abstract_world_manager() = default;

    std::shared_ptr<core::abstract_maze> maze_ {nullptr};

protected:
    std::vector<std::shared_ptr<game_object>> game_objects;
};

}

#endif // ABSTRACT_WORLD_MANAGER_HPP
