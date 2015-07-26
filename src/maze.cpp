#include "maze.hpp"
#include <fstream>
#include <iterator>
#include <cassert>
#include <climits>

#include "renderer.hpp"

/*
 * In maze::load_from_file std::copy + std::istream_iterator + back_inserter can't be used
   because every block of 'X' is treated as separeted string but I would like to rather
   whole line per string. So std::getline.
*/

namespace core
{

maze::maze(std::shared_ptr<presentation::renderer> renderer_)
 : renderer(renderer_)
{
}

void maze::load_from_file(const std::string &file_name)
{
    std::ifstream input_file(file_name);
    std::string line;
    while (std::getline(input_file, line))
    {
        content.push_back(line);
    }
    input_file.close();
}

bool maze::is_field_filled(int column, int row) const
{
//    assert((0 <= column) && (column < content.size()));
//    assert((0 <= row) && (row < content.size()));
    if (!((0 <= column) && (column < content.size())))
        return true;
    if (!((0 <= row) && (row < content.size())))
        return true;

    return content[column][row] != ' ';
}

char maze::get_field(int column, int row) const
{
    assert((0 <= column) && (column < content.size()));
    assert((0 <= row) && (row < content.size()));
    return content[column][row];
}

void maze::move_field(int column, int row, int new_column, int new_row)
{
    assert((0 <= column) && (column < content.size()));
    assert((0 <= row) && (row < content.size()));
    assert((0 <= new_column) && (new_column < content.size()));
    assert((0 <= new_row) && (new_row < content.size()));
    content[new_column][new_row] = content[column][row];
    content[column][row] = ' ';
}

void maze::reset_field(int column, int row)
{
    assert((0 <= column) && (column < content.size()));
    assert((0 <= row) && (row < content.size()));
    content[column][row] = ' ';
}

int maze::size()
{
    return content.size();
}

void maze::load()
{
    load_from_file("maze.txt");
    renderer->load_image_and_register("brick", "../../data/brick.bmp");
}

void maze::tick()
{
}

void maze::draw()
{
    const int brick_size = 30;
    for (int row = 0; row < size(); row++)
        for (int column = 0; column < size(); column++)
        {
            if (is_field_filled(column, row))
            {
                int posx = column * brick_size;
                int posy = row * brick_size;

                renderer->draw_image("brick", posx, posy);
            }
        }
}

std::tuple<int, int> maze::get_position() const
{
    return std::make_tuple(INT_MAX, INT_MAX);
}

}
