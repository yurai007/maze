#include "client_world_manager.hpp"

namespace core
{

client_world_manager::client_world_manager(std::shared_ptr<game_objects_factory> objects_factory_)
    : abstract_world_manager(objects_factory_)
{
    logger_.log("client_world_manager: started");
}

void client_world_manager::preprocess_loading()
{
     // HERE: On client I need extra phase here to obtain id-s
}

void client_world_manager::postprocess_loading()
{
    // HERE: On client extra phase to loading game_objects

    for (auto &object : game_objects)
        object->load();
}

void client_world_manager::preprocess_ticking()
{
    // on client extra get_chunks and get_enemies
}

void client_world_manager::draw_all()
{
    assert(maze_ != nullptr);
    maze_->draw();

    for (auto &object : game_objects)
        if (object != nullptr)
            object->draw();
}

}


