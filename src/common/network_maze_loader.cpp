#include "network_maze_loader.hpp"
#include "../client/client.hpp"

namespace networking
{

network_maze_loader::network_maze_loader(std::shared_ptr<client> maze_client)
    : m_maze_client(maze_client)
{
}


std::vector<std::string> network_maze_loader::load()
{
    messages::get_chunk request;
    m_maze_client->send_request(request);

    auto response = m_maze_client->read_get_chunk_response();

}
}
