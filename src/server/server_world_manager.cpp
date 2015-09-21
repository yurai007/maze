#include <cassert>
#include <climits>

#include "server_world_manager.hpp"
#include "../common/logger.hpp"

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

void server_world_manager::make_enemy(int posx, int posy)
{
    assert(maze_ != nullptr);
    game_objects.push_back(objects_factory->create_server_enemy(posx, posy));
    logger_.log("client_world_manager: added enemy on position = {%d, %d}", posx, posy);
}

void server_world_manager::make_player(int posx, int posy)
{
    game_objects.push_back(objects_factory->create_server_player(posx, posy));
    logger_.log("client_world_manager: added player on position = {%d, %d}", posx, posy);
}

void server_world_manager::make_resource(const std::string &name, int posx, int posy)
{
    game_objects.push_back(objects_factory->create_server_resource(name, posx, posy));
    logger_.log("client_world_manager: added %s on position = {%d, %d}", name.c_str(), posx, posy);
}

std::vector<int> server_world_manager::get_enemies_data(bool verify) const
{
    std::vector<int> result;
    for (auto &object : game_objects)
    {
        auto enemy_ = std::dynamic_pointer_cast<server_enemy>(object);
        if (enemy_ != nullptr)
        {
            auto position = enemy_->get_position();

            if (verify)
            {
                char field = maze_->get_field(std::get<0>(position), std::get<1>(position));
                assert(field == 'E');
            }

            result.push_back(enemy_->get_id());
            result.push_back(std::get<0>(position));
            result.push_back(std::get<1>(position));
        }
    }
    return result;
}

}
