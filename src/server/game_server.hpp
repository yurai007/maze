#ifndef GAME_SERVER_HPP
#define GAME_SERVER_HPP

#include <memory>
#include "../common/messages.hpp"
#include "server_maze.hpp"
#include "server.h"
#include "remote_transport.hpp"
#include "server_world_manager.hpp"

namespace networking
{

class game_server
{
public:
    game_server() = default;
    void init(std::shared_ptr<core::server_maze> maze,
              std::shared_ptr<core::server_world_manager> manager);
    void run();
    void stop();

     server main_server {5555};

private:

    remote_transport::sender sender {main_server};
};

}

#endif // GAME_SERVER_HPP
