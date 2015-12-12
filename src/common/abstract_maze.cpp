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
    if ( content[column][row] == 'G' || content[column][row] == 'W' || content[column][row] == 'M'
         || content[column][row] == 's' || content[column][row] == 'S')
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

void abstract_maze::set_field(int column, int row, char field)
{
    std::lock_guard<std::mutex> lock(maze_mutex);

    const int size = static_cast<int>(content.size());
    assert((0 <= column) && (column < size));
    assert((0 <= row) && (row < size));
    assert(field == 'P' || field == 'E' || field == 'G' || field == 'W' || field == 'M'
           || field == 's' || field == 'S');
    content[column][row] = field;
}

std::string abstract_maze::get_chunk(unsigned leftdown_x, unsigned leftdown_y,
                                         unsigned rightupper_x, unsigned rightupper_y) const
{
    std::lock_guard<std::mutex> lock(maze_mutex);
    // IV cw
    assert(leftdown_x < content.size() );
    assert(leftdown_x <= rightupper_x);
    assert(leftdown_y >= rightupper_y);
    const unsigned length = rightupper_x - leftdown_x + 1;
    std::string result;
    for (size_t i = rightupper_y; i <= leftdown_y; i++)
        result += content[i].substr(leftdown_x, length);
    return result;
}

void abstract_maze::move_field(const std::tuple<int, int> old_pos,
                               const std::tuple<int, int> new_pos)
{
    int new_column = std::get<0>(new_pos);
    int new_row = std::get<1>(new_pos);
    int column = std::get<0>(old_pos);
    int row = std::get<1>(old_pos);

    std::lock_guard<std::mutex> lock(maze_mutex);

    const int size = static_cast<int>(content.size());
    assert((0 <= column) && (column < size));
    assert((0 <= row) && (row < size));
    assert((0 <= new_column) && (new_column < size));
    assert((0 <= new_row) && (new_row < size));
    content[new_column][new_row] = content[column][row];
    content[column][row] = ' ';
}

void abstract_maze::reset_field(const std::tuple<int, int> pos)
{
    int column = std::get<0>(pos);
    int row = std::get<1>(pos);

    std::lock_guard<std::mutex> lock(maze_mutex);

    const int size = static_cast<int>(content.size());
    assert((0 <= column) && (column < size));
    assert((0 <= row) && (row < size));
    content[column][row] = ' ';
}

int abstract_maze::size() const
{
    std::lock_guard<std::mutex> lock(maze_mutex);
    return content.size();
}

void abstract_maze::update_content()
{
    std::lock_guard<std::mutex> lock(maze_mutex);

    auto temporary_content = m_loader->load();

//    for (size_t i = 0; i < temporary_content.size(); i++)
//    {
//        if (temporary_content[i] != content[i])
//            logger_.log("row %d: %s", i, temporary_content[i].c_str());
//    }
    content = temporary_content;
    logger_.log("abstract_maze: content was load");
}

void abstract_maze::verify() const
{
    std::lock_guard<std::mutex> lock(maze_mutex);

    for (size_t i = 0; i < content.size(); i++)
    {
        assert(content[i].size() == 60);
    }
}

void abstract_maze::tick(unsigned short)
{
}

}
