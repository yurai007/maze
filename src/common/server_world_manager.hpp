#ifndef SERVER_WORLD_MANAGER_HPP
#define SERVER_WORLD_MANAGER_HPP

#include <vector>
#include <memory>
#include "abstract_world_manager.hpp"

namespace core
{

class server_world_manager : public abstract_world_manager
{
public:
    server_world_manager(std::shared_ptr<game_objects_factory>
                         objects_factory_);

    void preprocess_loading() override;
    void postprocess_loading() override;
    void preprocess_ticking() override;
    void make_enemy(int posx, int posy) override;

    void draw_all();
    std::vector<int> get_enemies_data(bool verify) const;
    void verify_maze_with_game_objects();
};

}

#endif // SERVER_WORLD_MANAGER_HPP
