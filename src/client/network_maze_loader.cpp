#include "network_maze_loader.hpp"
#include "../client/client.hpp"

namespace networking
{

network_maze_loader::network_maze_loader(smart::fit_smart_ptr<client> maze_client)
    : m_maze_client(maze_client)
{
}

std::vector<std::string> network_maze_loader::load()
{
    assert(m_maze_client != nullptr);

    constexpr unsigned maze_size = 60;
    constexpr unsigned max_msg_size = 256;
    constexpr unsigned d = (maze_size*maze_size)/max_msg_size;
    constexpr unsigned rows_number = maze_size/d;
    static_assert(maze_size % rows_number == 0, "Maze can't be obtained in equal chunks");

    std::vector<std::string> result;

    for (unsigned i = 0; i < maze_size; i += rows_number)
    {
        const unsigned ld_x = 0, ld_y = i + rows_number - 1;
        const unsigned ru_x = maze_size - 1, ru_y = i;

        messages::get_chunk request = {ld_x, ld_y, ru_x, ru_y};
        m_maze_client->send_request(request);

        const auto response = m_maze_client->read_get_chunk_response();

//        logger_.log_debug("response.content size = %zu", response.content.size());
//        logger_.log_debug("response.content %d: %s", i, response.content.c_str());

        for (unsigned j = 0; j < rows_number; j++)
        {
            const auto row = response.content.substr(2*j*maze_size, 2*maze_size);
            assert(row.size() == 2*maze_size);
            result.push_back(row);
        }
    }
    logger_.log("network_maze_loader: recieved maze from server");
    return result;
}

}
