#ifndef SERVER_MAZE_HPP
#define SERVER_MAZE_HPP

#include "../common/abstract_maze.hpp"
#include "../common/smart_ptr.hpp"

namespace core
{

class server_maze : public abstract_maze
{
public:
    server_maze(smart::fit_smart_ptr<core::maze_loader> loader);

    void load();
};

}

#endif // SERVER_MAZE_HPP
