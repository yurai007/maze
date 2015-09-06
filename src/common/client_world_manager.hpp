#ifndef CLIENT_WORLD_MANAGER_HPP
#define CLIENT_WORLD_MANAGER_HPP

#include "abstract_world_manager.hpp"

namespace core
{

class client_world_manager : public abstract_world_manager
{
public:
    client_world_manager(std::shared_ptr<game_objects_factory>
                         objects_factory_);

    void preprocess_loading() override;
    void postprocess_loading() override;
    void preprocess_ticking() override;

    void draw_all();
};

}

#endif // CLIENT_WORLD_MANAGER_HPP
