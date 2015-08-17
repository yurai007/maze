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

    std::lock_guard<std::mutex> lock(maze_mutex);
    while (std::getline(input_file, line))
    {
        content.push_back(line);
    }
    input_file.close();
}

bool maze::is_field_filled(int column, int row) const
{
    std::lock_guard<std::mutex> lock(maze_mutex);
    if (!((0 <= column) && (column < content.size())))
        return true;
    if (!((0 <= row) && (row < content[column].size())))
        return true;
    if ( content[column][row] == 'G')
        return false;
    return content[column][row] != ' ';
}

char maze::get_field(int column, int row) const
{
    std::lock_guard<std::mutex> lock(maze_mutex);
    assert((0 <= column) && (column < content.size()));
    assert((0 <= row) && (row < content[column].size()));
    return content[column][row];
}

std::string maze::get_chunk(int leftdown_x, int leftdown_y,
                                         int rightupper_x, int rightupper_y) const
{
    std::lock_guard<std::mutex> lock(maze_mutex);
    // IV cw
    assert(0 <= leftdown_x && leftdown_x < content.size());
    assert(leftdown_x >= rightupper_x);
    assert(leftdown_y <= rightupper_y);
    int length = rightupper_y - leftdown_y + 1;
    std::string result;
    for (size_t i = rightupper_x; i <= leftdown_x; i++)
        result += content[i].substr(leftdown_y, length);
    return result;
}

void maze::move_field(int column, int row, int new_column, int new_row)
{
    std::lock_guard<std::mutex> lock(maze_mutex);

    assert((0 <= column) && (column < content.size()));
    assert((0 <= row) && (row < content.size()));
    assert((0 <= new_column) && (new_column < content.size()));
    assert((0 <= new_row) && (new_row < content.size()));
    content[new_column][new_row] = content[column][row];
    content[column][row] = ' ';
}

void maze::reset_field(int column, int row)
{
    std::lock_guard<std::mutex> lock(maze_mutex);
    assert((0 <= column) && (column < content.size()));
    assert((0 <= row) && (row < content.size()));
    content[column][row] = ' ';
}

int maze::size()
{
    std::lock_guard<std::mutex> lock(maze_mutex);
    return content.size();
}

void maze::load()
{
    load_from_file("maze.txt");
    std::lock_guard<std::mutex> lock(maze_mutex);
    renderer->load_image_and_register("brick", "../../data/brick.bmp");
}

void maze::tick(unsigned short tick_counter)
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

                std::lock_guard<std::mutex> lock(maze_mutex);
                renderer->draw_image("brick", posx, posy);
            }
        }
}

std::tuple<int, int> maze::get_position() const
{
    return std::make_tuple(INT_MAX, INT_MAX);
}

}
