#include "abstract_world_manager.hpp"
#include "remote_player.hpp"
#include "resource.hpp"

namespace core
{

abstract_world_manager::abstract_world_manager(std::shared_ptr<game_objects_factory>
                                               objects_factory_)
    : objects_factory(objects_factory_)
{
}

void abstract_world_manager::add_maze(std::shared_ptr<maze_loader> loader)
{
    maze_ = objects_factory->create_maze(loader);
    logger_.log("abstract_world_manager: added maze");
}

void abstract_world_manager::add_remote_player(int posx, int posy)
{
    assert(maze_ != nullptr);
    game_objects.push_back(objects_factory->create_remote_player(posx, posy));
    logger_.log("abstract_world_manager: added player on position = {%d, %d}", posx, posy);
}

void abstract_world_manager::add_enemy(int posx, int posy)
{
    assert(maze_ != nullptr);
    game_objects.push_back(objects_factory->create_enemy(posx, posy));
    logger_.log("abstract_world_manager: added enemy on position = {%d, %d}", posx, posy);
}

void abstract_world_manager::add_resource(const std::string &name, int posx, int posy)
{
    game_objects.push_back(objects_factory->create_resource(name, posx, posy));
    logger_.log("abstract_world_manager: added %s on position = {%d, %d}", name.c_str(), posx, posy);
}

void abstract_world_manager::load_all()
{
    maze_->load();

    // HERE: On client I need extra phase here to obtain id-s
    preprocess_loading();

    for (int row = 0; row < maze_->size(); row++)
        for (int column = 0; column < maze_->size(); column++)
        {
            char field = maze_->get_field(column, row);
            if (field == 'P')
                add_remote_player(column, row);
            else
                if (field == 'E')
                    add_enemy(column, row);
                else
                    if (field == 'G')
                        add_resource("gold", column, row);
        }

    // HERE: On client extra phase to loading game_objects

//    if (renderer == nullptr)
//        return;

//    for (auto &object : game_objects)
//        object->load();
    postprocess_loading();

    logger_.log("abstract_world_manager: all game objects were loaded successfully");
}

void abstract_world_manager::tick_all()
{
    static unsigned short tick_counter = 0;

    // on client extra get_chunks and get_enemies
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
                if (std::dynamic_pointer_cast<resource>(object) != nullptr)
                    // for resource client: get_chunk
                    if (maze_->get_field(std::get<0>(old_position), std::get<1>(old_position)) != 'G')
                    {
                        object.reset();
                        logger_.log("abstract_world_manager: removed resource from positon = {%d, %d}",
                            std::get<0>(old_position), std::get<1>(old_position));
                    }
            }
    }
    if (tick_counter%10 == 0)
        logger_.log("abstract_world_manager: finished tick with id = %d successfully", tick_counter);
    tick_counter++;
}


}
