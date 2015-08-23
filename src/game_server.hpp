#ifndef GAME_SERVER_HPP
#define GAME_SERVER_HPP

#include <memory>
#include "maze.hpp"
//#include "byte_buffer.hpp"
#include "server.h"
#include "remote_transport.hpp"
#include "messages.hpp"

namespace networking
{

class game_server
{
public:
    game_server();
    void init(std::shared_ptr<core::maze> maze);
    void run();
    void stop();

private:
    server main_server {5555};
    remote_transport::sender sender {main_server};
};

}

#endif // GAME_SERVER_HPP
