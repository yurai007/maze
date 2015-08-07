#ifndef GAME_SERVER_HPP
#define GAME_SERVER_HPP

#include "maze.hpp"
#include <memory>

namespace networking
{

class game_server
{
public:
    game_server(std::shared_ptr<core::maze> maze);
};

}

#endif // GAME_SERVER_HPP
