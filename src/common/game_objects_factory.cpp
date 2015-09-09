#include "renderer.hpp"
#include "controller.hpp"
#include "../client/client.hpp"
#include "game_objects_factory.hpp"

namespace core
{

game_objects_factory::game_objects_factory(std::shared_ptr<presentation::renderer> renderer,
                     std::shared_ptr<control::controller> controller,
                     std::shared_ptr<networking::client> client)
    : renderer_(renderer),
      controller_(controller),
      client_(client)
{
}

std::shared_ptr<maze> game_objects_factory::create_maze(std::shared_ptr<maze_loader> loader)
{
    maze_ = std::make_shared<maze>(renderer_, loader);
    return maze_;
}

std::shared_ptr<remote_player> game_objects_factory::create_remote_player(int posx, int posy)
{
    return std::make_shared<remote_player>(renderer_, controller_, maze_, client_, posx, posy);
}

std::shared_ptr<enemy> game_objects_factory::create_enemy(int posx, int posy)
{
    return std::make_shared<enemy>(renderer_, maze_, posx, posy);
}

std::shared_ptr<client_enemy> game_objects_factory::create_client_enemy(
                std::shared_ptr<abstract_world_manager> manager,
                int posx, int posy, int id)
{
    return std::make_shared<client_enemy>(manager, renderer_, maze_, posx, posy, id);
}

std::shared_ptr<resource> game_objects_factory::create_resource(const std::string &name,
                                                                int posx, int posy)
{
    return std::make_shared<resource>(name, renderer_, posx, posy);
}

}


