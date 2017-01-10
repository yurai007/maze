#ifndef GAME_SERVER_HPP
#define GAME_SERVER_HPP

#include "../common/smart_ptr.hpp"

#include "server_maze.hpp"
#include "server.hpp"
#include "server_world_manager.hpp"

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
    io_service &get_io_service();

private:
    server main_server {port_number};
};

}

#endif // GAME_SERVER_HPP
