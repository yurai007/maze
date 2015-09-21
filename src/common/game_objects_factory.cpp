#include "../client/renderer.hpp"
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

std::shared_ptr<client_player> game_objects_factory::create_client_player(int posx, int posy)
{
    return std::make_shared<client_player>(renderer_, controller_, maze_, client_, posx, posy);
}

std::shared_ptr<server_player> game_objects_factory::create_server_player(int posx, int posy)
{
    return std::make_shared<server_player>(maze_, posx, posy);
}

std::shared_ptr<server_enemy> game_objects_factory::create_server_enemy(int posx, int posy)
{
    return std::make_shared<server_enemy>(maze_, posx, posy);
}

std::shared_ptr<client_enemy> game_objects_factory::create_client_enemy(
                std::shared_ptr<client_world_manager> manager,
                int posx, int posy, int id)
{
    return std::make_shared<client_enemy>(manager, renderer_, maze_, posx, posy, id);
}

std::shared_ptr<client_resource> game_objects_factory::create_client_resource(const std::string &name,
                                                                int posx, int posy)
{
    return std::make_shared<client_resource>(name, renderer_, posx, posy);
}

std::shared_ptr<server_resource> game_objects_factory::create_server_resource(const std::string &name,
                                                               int posx, int posy)
{
    return std::make_shared<server_resource>(name, posx, posy);
}

}


