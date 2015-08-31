#ifndef FILE_MAZE_LOADER_HPP
#define FILE_MAZE_LOADER_HPP

#include "maze_loader.hpp"

namespace core
{

class file_maze_loader : public maze_loader
{
public:
    file_maze_loader() = default;
    std::vector<std::string> load() override;

private:
    std::vector<std::string> load_from_file(const std::string &file_name);
};

}
#endif // FILE_MAZE_LOADER_HPP
