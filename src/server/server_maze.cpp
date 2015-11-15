#include "server_maze.hpp"
#include "../common/logger.hpp"

namespace core
{

server_maze::server_maze(std::shared_ptr<maze_loader> loader)
    : abstract_maze(loader)
{
}

void server_maze::load()
{
    std::lock_guard<std::mutex> lock(maze_mutex);

    content = m_loader->load();
    logger_.log("server_maze: content was load");

//    for (size_t i = 0; i < content.size(); i++)
//        logger_.log("row %d: %s", i, content[i].c_str());
}

}
