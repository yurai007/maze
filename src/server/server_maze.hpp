#ifndef SERVER_MAZE_HPP
#define SERVER_MAZE_HPP

#include "../common/abstract_maze.hpp"

namespace core
{

class server_maze : public abstract_maze
{
public:
    server_maze(std::shared_ptr<core::maze_loader> loader);

    void load() override;
};

}

#endif // SERVER_MAZE_HPP
