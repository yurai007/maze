#include <cassert>
#include <climits>

#include "server_world_manager.hpp"
#include "remote_player.hpp"
#include "enemy.hpp"
#include "resource.hpp"
#include "renderer.hpp"


namespace core
{

server_world_manager::server_world_manager(std::shared_ptr<game_objects_factory> objects_factory_)
    : abstract_world_manager(objects_factory_)
{
    logger_.log("server_world_manager: started");
}

void server_world_manager::preprocess_loading()
{
}

void server_world_manager::postprocess_loading()
{
}

void server_world_manager::preprocess_ticking()
{
}

void server_world_manager::draw_all()
{
}

}
