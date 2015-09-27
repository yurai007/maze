#include <cassert>
#include <climits>
#include "abstract_world_manager.hpp"
#include "../server/server_resource.hpp"
#include "../client/client_resource.hpp"
#include "logger.hpp"

namespace core
{

void abstract_world_manager::load_all()
{
    logger_.log("abstract_world_manager: start loading");
    assert(maze_ != nullptr);
    maze_->load();

    preprocess_loading();

    for (int row = 0; row < maze_->size(); row++)
        for (int column = 0; column < maze_->size(); column++)
        {
            char field = maze_->get_field(column, row);
            if (field == 'P')
                make_player(column, row);
            else
                if (field == 'E')
                    make_enemy(column, row);
                else
                    if (field == 'G')
                        make_resource("gold", column, row);
        }

    postprocess_loading();

    logger_.log("abstract_world_manager: all game objects were loaded successfully");
}

void abstract_world_manager::tick_all(bool omit_moving_fields)
{
    static unsigned short tick_counter = 0;
    logger_.log("abstract_world_manager: started tick with id = %d", tick_counter);
    preprocess_ticking();

    maze_->tick(tick_counter);
    for (auto &object : game_objects)
        if (object != nullptr)
        {
            auto old_position = object->get_position();
            object->tick(tick_counter);
            auto new_position = object->get_position();

            if (new_position != old_position)
            {
                if (omit_moving_fields)
                    continue;

                int new_pos_x = std::get<0>(new_position);
                int new_pos_y = std::get<1>(new_position);

                int old_pos_x = std::get<0>(old_position);
                int old_pos_y = std::get<1>(old_position);

                if (new_pos_x < INT_MAX)
                    maze_->move_field(old_pos_x, old_pos_y, new_pos_x, new_pos_y);
                else
                    maze_->reset_field(old_pos_x, old_pos_y);
            }
            else
            {
                // dirty hack, downcasting for zombie
                if (check_if_resource(object))
                    // for resource client: get_chunk
                    if (maze_->get_field(std::get<0>(old_position), std::get<1>(old_position)) != 'G')
                    {
                        object.reset();
                        logger_.log("abstract_world_manager: removed resource from positon = {%d, %d}",
                            std::get<0>(old_position), std::get<1>(old_position));
                    }
            }
    }
    logger_.log("abstract_world_manager: finished tick with id = %d", tick_counter);
    tick_counter++;
}


}
