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

abstract_maze::abstract_maze(smart::fit_smart_ptr<maze_loader> loader)
    : game_object(INT_MAX, INT_MAX),
      m_loader(loader)
{
}

bool abstract_maze::is_field_filled(int column, int row) const
{
    if (!((0 <= column) && (column < static_cast<int>(content.size()) )))
        return true;
    if (!((0 <= row) && (row < static_cast<int>(column_size(0)) )))
        return true;
    auto field = get_field(column, row);
    if ( field == 'G' || field == 'W' || field == 'M' || field == 's' || field == 'S')
        return false;
    return field != ' ';
}

char abstract_maze::get_field(int column, int row) const
{
    auto orig_field = get_extended_field(column, row);
    return std::get<0>(to_normal(orig_field));
}

unsigned short abstract_maze::get_id(int column, int row) const
{
    auto orig_field = get_extended_field(column, row);
    return std::get<1>(to_normal(orig_field));
}

void abstract_maze::set_field(int column, int row, char field)
{
    const int size = static_cast<int>(content.size());
    assert((0 <= column) && (column < size));
    assert((0 <= row) && (row < size));
    assert(field == 'P' || field == 'E' || field == 'G' || field == 'W' || field == 'M'
           || field == 's' || field == 'S' || field == ' ');
    auto out_field = to_extended(field, 0);
    content[column][2*row] = out_field & 0xFF;
    content[column][2*row+1] = (out_field >> 8);
}

std::string abstract_maze::get_chunk(unsigned leftdown_x, unsigned leftdown_y,
                                         unsigned rightupper_x, unsigned rightupper_y) const
{
    // IV cw
    assert(leftdown_x < content.size() );
    assert(leftdown_x <= rightupper_x);
    assert(leftdown_y >= rightupper_y);
    const unsigned length = rightupper_x - leftdown_x + 1;
    std::string result;
    for (size_t i = rightupper_y; i <= leftdown_y; i++)
    {
        auto sub_column = content[i].substr(2*leftdown_x, 2*length);
        result += sub_column;
    }
    return result;
}

void abstract_maze::move_field(const std::tuple<int, int> old_pos,
                               const std::tuple<int, int> new_pos)
{
    int new_column = std::get<0>(new_pos);
    int new_row = std::get<1>(new_pos);
    int column = std::get<0>(old_pos);
    int row = std::get<1>(old_pos);

    const int size = static_cast<int>(content.size());
    assert((0 <= column) && (column < size));
    assert((0 <= row) && (row < size));
    assert((0 <= new_column) && (new_column < size));
    assert((0 <= new_row) && (new_row < size));

    set_extended_field(new_column, new_row, get_extended_field(column, row));
    set_extended_field(column, row, to_extended(' ', 0));

//    set_field(new_column, new_row, get_field(column, row));
//    set_field(column, row, ' ');
}

void abstract_maze::reset_field(const std::tuple<int, int> pos)
{
    int column = std::get<0>(pos);
    int row = std::get<1>(pos);

    const int size = static_cast<int>(content.size());
    assert((0 <= column) && (column < size));
    assert((0 <= row) && (row < size));
    set_field(column, row, ' ');
}

int abstract_maze::size() const
{
    return content.size();
}

int abstract_maze::column_size(int column) const
{
    return content[column].size()/2;
}

void abstract_maze::update_content()
{
    assert(m_loader != nullptr);
    content = m_loader->load();
}

void abstract_maze::verify() const
{
    for (size_t i = 0; i < content.size(); i++)
    {
        assert(column_size(i) == 60);
    }
}

void abstract_maze::tick(unsigned short)
{
}

unsigned short abstract_maze::get_extended_field(int column, int row) const
{
    assert((0 <= column) && (column < static_cast<int>(content.size()) ));
    assert((0 <= row) && (row < static_cast<int>(column_size(0)) ));
    unsigned char high = content[column][2*row+1];
    unsigned char low = content[column][2*row];
    return (unsigned short)(high)<<8 | low;;
}

void abstract_maze::set_extended_field(int column, int row, unsigned short field)
{
    assert((0 <= column) && (column < static_cast<int>(content.size()) ));
    assert((0 <= row) && (row < static_cast<int>(column_size(0)) ));
    content[column][2*row] = field & 0xFF;
    content[column][2*row+1] = (field >> 8);
}
}
