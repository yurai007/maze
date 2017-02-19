#include "server_maze.hpp"
#include "../common/logger.hpp"

namespace core
{

server_maze::server_maze(smart::fit_smart_ptr<maze_loader> loader)
    : abstract_maze(loader)
{}

void server_maze::load()
{
    content = m_loader->load();
    logger_.log("server_maze: content was load");
    logger_.log_debug("server_maze: Content:");

    for (size_t i = 0; i < content.size(); i++)
        logger_.log_debug("row %d: %s", i, content[i].c_str());

    std::array<unsigned short, 6> counters = {0,0,0,0,0,0};//PEX''FR

    for (unsigned i = 0; i < content.size(); i++)
    {
        content[i] = to_extended(content[i], [&counters](auto field){

            assert(field == 'P' || field == 'E' || field == 'G' || field == 'W' || field == 'M'
                   || field == 's' || field == 'S' || field == 'X' || field == ' ' || field == 'F');
            if (field == 'P')
                return counters[0]++;
            else
                if (field == 'E')
                    return counters[1]++;
                else
                    if (field == 'X')
                        return counters[2]++;
                    else
                        if (field == ' ')
                            return counters[3]++;
                        else
                            if (field == 'F')
                                return counters[4]++;
                            else
                                return counters[5]++;
        });
    }
    logger_.log("server_maze: Counters: P=%u, E=%u, X=%u, ' '=%u, F=%u, R=%u",
                      counters[0], counters[1], counters[2], counters[3], counters[4], counters[5]);

    logger_.log_debug("server_maze: Extended content focused on enemies:");

    for (size_t i = 0; i < content.size(); i++)
    {
        std::string column = "";
        for (size_t j = 0; j < column_size(i); j++)
        {
            auto field = get_field(i, j);
            auto id = get_id(i, j);
            if (field == 'E')
            {
                if (id < 10)
                    column += std::to_string(id)[0];
                else
                    column += field;
            }
            else
                column += field;
        }
        logger_.log_debug("row %d: %s", i, column.c_str());
    }
}

}
