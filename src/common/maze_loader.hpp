#ifndef MAZE_LOADER_HPP
#define MAZE_LOADER_HPP

#include <vector>
#include <string>

namespace core
{

class maze_loader
{
public:
    virtual std::vector<std::string> load() = 0;
};

}

#endif // MAZE_LOADER_HPP
