#include "../client/network_manager.hpp"
#include "client_game_objects_factory.hpp"

#include "../client/renderer.hpp"
#include "controller.hpp"

/*
 * How to force gcc to make inifinite compilation (inifinite loop)?
   Exchange client_game_objects_factory.hpp with client_game_objects_factory.cpp content. Then
   client_game_objects_factory.hpp include themself without include guard and including takes forever :)
 */

namespace core
{

client_game_objects_factory::client_game_objects_factory(
                     smart::fit_smart_ptr<presentation::renderer> renderer,
                     smart::fit_smart_ptr<control::controller> controller,
                     smart::fit_smart_ptr<networking::network_manager> network_manager)
    : renderer_(renderer),
      controller_(controller),
      network_manager_(network_manager)
{
}

smart::fit_smart_ptr<client_maze> client_game_objects_factory::create_client_maze(
        smart::fit_smart_ptr<maze_loader> loader,
        bool visible)
{
    maze_ = smart::smart_make_shared<client_maze>(renderer_, loader, visible);
    return maze_;
}

smart::fit_smart_ptr<client_player> client_game_objects_factory::create_client_player(
                                        client_world_manager &manager,
                                        int id, int posx, int posy, bool active, bool automatic)
{
    return smart::smart_make_shared<client_player>(manager, renderer_, controller_, maze_,
                                           network_manager_, id, posx, posy, active, automatic);
}

smart::fit_smart_ptr<client_enemy> client_game_objects_factory::create_client_enemy(
                int posx, int posy, int id)
{
    return smart::smart_make_shared<client_enemy>(renderer_, posx, posy, id);
}

smart::fit_smart_ptr<client_resource> client_game_objects_factory::create_client_resource(
        const std::string &name,
        int posx, int posy)
{
    return smart::smart_make_shared<client_resource>(name, renderer_, posx, posy);
}

}
