#ifndef ABSTRACT_WORLD_MANAGER_HPP
#define ABSTRACT_WORLD_MANAGER_HPP

#include <vector>
#include <memory>

#include "game_object.hpp"
#include "maze_loader.hpp"
#include "maze.hpp"
#include "game_objects_factory.hpp"

namespace core
{

class abstract_world_manager : public std::enable_shared_from_this<abstract_world_manager>
{
public:
    abstract_world_manager(std::shared_ptr<game_objects_factory> objects_factory_);

    virtual void preprocess_loading() = 0;
    virtual void postprocess_loading() = 0;
    virtual void preprocess_ticking() = 0;
    virtual void make_enemy(int posx, int posy) = 0;
    virtual void make_player(int posx, int posy) = 0;
    virtual void make_resource(const std::string &name, int posx, int posy) = 0;

    void add_maze(std::shared_ptr<maze_loader> loader);
//    void add_client_player(int posx, int posy);
//    void add_enemy(int posx, int posy);
//    void add_client_enemy(int posx, int posy, int id);
//    void add_resource(const std::string &name, int posx, int posy);
    void tick_all(bool omit_moving_fields);
    void load_all();
    virtual ~abstract_world_manager() = default;

    std::shared_ptr<core::maze> maze_ {nullptr};

protected:
    std::vector<std::shared_ptr<game_object>> game_objects;
    std::shared_ptr<game_objects_factory> objects_factory {nullptr};
};

}

#endif // ABSTRACT_WORLD_MANAGER_HPP
