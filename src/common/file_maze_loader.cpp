#include <fstream>
#include <iterator>
#include "file_maze_loader.hpp"

namespace core
{

std::vector<std::string> file_maze_loader::load()
{
    return load_from_file("maze.txt");
}

std::vector<std::string> file_maze_loader::load_from_file(const std::string &file_name)
{
    std::ifstream input_file(file_name);
    std::string line;
    std::vector<std::string> content;

    while (std::getline(input_file, line))
    {
        content.push_back(line);
    }
    input_file.close();
    return content;
}

}
