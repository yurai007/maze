#ifndef MAZE_HPP
#define MAZE_HPP

#include <string>
#include <vector>
#include <memory>
#include "game_object.hpp"

namespace presentation
{
    class renderer;
}

namespace core
{

class maze : public game_object
{
public:
    maze(std::shared_ptr<presentation::renderer> renderer_);
    void load_from_file(const std::string &file_name);
    bool is_field_filled(int column, int row);
    int size();
    void load() override;
    void tick() override;
    void draw() override;

private:
    std::shared_ptr<presentation::renderer> renderer;
    std::vector<std::string> content;
};

}

#endif // MAZE_HPP
