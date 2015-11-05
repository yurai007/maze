#include "../client/renderer.hpp"
#include "../common/controller.hpp"
#include "../client/client.hpp"
#include "client_game_objects_factory.hpp"

/*
 * How to force gcc to make inifinite compilation (inifinite loop)?
   Exchange client_game_objects_factory.hpp with client_game_objects_factory.cpp content. Then
   client_game_objects_factory.hpp include themself without include guard and including takes forever :)
 */

namespace core
{

client_game_objects_factory::client_game_objects_factory(
                     std::shared_ptr<presentation::renderer> renderer,
                     std::shared_ptr<control::controller> controller,
                     std::shared_ptr<networking::client> client)
    : renderer_(renderer),
      controller_(controller),
      client_(client)
{
}

std::shared_ptr<client_maze> client_game_objects_factory::create_client_maze(
        std::shared_ptr<maze_loader> loader, bool visible)
{
    maze_ = std::make_shared<client_maze>(renderer_, loader, visible);
    return maze_;
}

std::shared_ptr<client_player> client_game_objects_factory::create_client_player(std::shared_ptr<client_world_manager> manager,
                                        int id, int posx, int posy, bool active, bool automatic)
{
    return std::make_shared<client_player>(manager, renderer_, controller_, maze_,
                                           client_, id, posx, posy, active, automatic);
}

std::shared_ptr<client_enemy> client_game_objects_factory::create_client_enemy(
                std::shared_ptr<client_world_manager> manager,
                int posx, int posy, int id)
{
    return std::make_shared<client_enemy>(manager, renderer_, maze_, posx, posy, id);
}

std::shared_ptr<client_resource> client_game_objects_factory::create_client_resource(
        const std::string &name,
        int posx, int posy)
{
    return std::make_shared<client_resource>(name, renderer_, posx, posy);
}

}
