#ifndef GAME_SERVER_HPP
#define GAME_SERVER_HPP

#include "../common/smart_ptr.hpp"

#include "server_maze.hpp"
#include "server.hpp"
#include "server_world_manager.hpp"

#include "asio_reactor.hpp"

namespace networking
{

constexpr static int port_number = 5555;

class game_server
{
public:
    game_server() = default;
    void init(smart::fit_smart_ptr<core::server_maze> maze,
              smart::fit_smart_ptr<core::server_world_manager> manager);
    void run();
    void stop();
    asio_reactor &get_reactor();

private:
    server<asio_reactor> main_server {port_number};
};

}

#endif // GAME_SERVER_HPP
