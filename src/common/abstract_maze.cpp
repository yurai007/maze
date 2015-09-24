#include <fstream>
#include <iterator>
#include <cassert>
#include <climits>

#include "abstract_maze.hpp"
#include "logger.hpp"

/*
 * In maze::load_from_file std::copy + std::istream_iterator + back_inserter can't be used
   because every block of 'X' is treated as separated string but I would like to rather
   whole line per string. So std::getline.
*/

namespace core
{

abstract_maze::abstract_maze(std::shared_ptr<maze_loader> loader)
 : game_object(INT_MAX, INT_MAX),
   m_loader(loader)
{
}

bool abstract_maze::is_field_filled(int column, int row) const
{
    std::lock_guard<std::mutex> lock(maze_mutex);

    if (!((0 <= column) && (column < static_cast<int>(content.size()) )))
        return true;
    if (!((0 <= row) && (row < static_cast<int>(content[column].size()) )))
        return true;
    if ( content[column][row] == 'G')
        return false;
    return content[column][row] != ' ';
}

char abstract_maze::get_field(int column, int row) const
{
    std::lock_guard<std::mutex> lock(maze_mutex);

    assert((0 <= column) && (column < static_cast<int>(content.size()) ));
    assert((0 <= row) && (row < static_cast<int>(content[column].size()) ));
    return content[column][row];
}

std::string abstract_maze::get_chunk(int leftdown_x, int leftdown_y,
                                         int rightupper_x, int rightupper_y) const
{
    std::lock_guard<std::mutex> lock(maze_mutex);
    // IV cw
    assert(0 <= leftdown_x && leftdown_x < static_cast<int>(content.size()) );
    assert(leftdown_x <= rightupper_x);
    assert(leftdown_y >= rightupper_y);
    int length = rightupper_x - leftdown_x + 1;
    std::string result;
    for (size_t i = rightupper_y; i <= leftdown_y; i++)
        result += content[i].substr(leftdown_x, length);
    return result;
}

void abstract_maze::move_field(int column, int row, int new_column, int new_row)
{
    std::lock_guard<std::mutex> lock(maze_mutex);

    const int size = static_cast<int>(content.size());
    assert((0 <= column) && (column < size));
    assert((0 <= row) && (row < size));
    assert((0 <= new_column) && (new_column < size));
    assert((0 <= new_row) && (new_row < size));
    content[new_column][new_row] = content[column][row];
    content[column][row] = ' ';
}

void abstract_maze::reset_field(int column, int row)
{
    std::lock_guard<std::mutex> lock(maze_mutex);

    const int size = static_cast<int>(content.size());
    assert((0 <= column) && (column < size));
    assert((0 <= row) && (row < size));
    content[column][row] = ' ';
}

int abstract_maze::size()
{
    std::lock_guard<std::mutex> lock(maze_mutex);
    return content.size();
}

void abstract_maze::update_content()
{
    std::lock_guard<std::mutex> lock(maze_mutex);

    auto temporary_content = m_loader->load();
    logger_.log("abstract_maze: content was load. Content dump diff:");
    for (size_t i = 0; i < temporary_content.size(); i++)
    {
        if (temporary_content[i] != content[i])
            logger_.log("row %d: %s", i, temporary_content[i].c_str());
    }

    content = temporary_content;
}

void abstract_maze::verify()
{
    std::lock_guard<std::mutex> lock(maze_mutex);

    for (size_t i = 0; i < content.size(); i++)
    {
        assert(content[i].size() == 50);
    }
}

void abstract_maze::tick(unsigned short)
{
}

}
