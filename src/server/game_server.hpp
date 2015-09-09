#ifndef GAME_SERVER_HPP
#define GAME_SERVER_HPP

#include <memory>
#include "../common/maze.hpp"
#include "server.h"
#include "remote_transport.hpp"
#include "../common/messages.hpp"
#include "../common/server_world_manager.hpp"

namespace networking
{

class game_server
{
public:
    game_server();
    void init(std::shared_ptr<core::maze> maze,
              std::shared_ptr<core::server_world_manager> manager);
    void run();
    void stop();

     server main_server {5555};

private:

    remote_transport::sender sender {main_server};
};

}

#endif // GAME_SERVER_HPP
