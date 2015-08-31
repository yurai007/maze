#ifndef MAZE_GENERATOR_HPP
#define MAZE_GENERATOR_HPP

#include <string>
#include <vector>

namespace utils
{

class maze_generator
{
public:
    maze_generator(int size);
    void generate_with_patterns_from(const std::string &file_name, int d);
    void save_to_file(const std::string &file_name);

private:
    void read_pattern(int i, std::ifstream &input_file);
    void apply_pattern_to_content(const std::vector<std::string> &pattern, int column, int row);
    size_t get_pattern_id() const;

    std::vector< std::string > content;
    std::vector<std::vector<std::string> > patterns;
};

}

#endif // MAZE_GENERATOR_HPP
