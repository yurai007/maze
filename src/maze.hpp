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
    bool is_field_filled(int column, int row) const;
    char get_field(int column, int row) const;

    std::vector<std::string> get_chunk(int leftdown_x, int leftdown_y,
                                       int rightupper_x, int rightupper_y) const;

    void move_field(int column, int row, int new_column, int new_row);
    void reset_field(int column, int row);
    int size();
    void load() override;
    void tick(unsigned short tick_counter) override;
    void draw() override;
    std::tuple<int, int> get_position() const override;

private:
    std::shared_ptr<presentation::renderer> renderer;
    std::vector<std::string> content;
};

}

#endif // MAZE_HPP
