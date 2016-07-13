#ifndef NETWORK_MAZE_LOADER_HPP
#define NETWORK_MAZE_LOADER_HPP

#include "../common/maze_loader.hpp"
#include "../common/smart_ptr.hpp"
#include "../client/client.hpp"

namespace networking
{

class network_maze_loader : public core::maze_loader
{
public:
    network_maze_loader(smart::fit_smart_ptr<client> maze_client);
    std::vector<std::string> load() override;

private:
    smart::fit_smart_ptr<client> m_maze_client;
};

}

#endif // NETWORK_MAZE_LOADER_HPP
