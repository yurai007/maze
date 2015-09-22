#include "maze.hpp"
#include <fstream>
#include <iterator>
#include <cassert>
#include <climits>

#include "../client/renderer.hpp"
#include "maze_loader.hpp"

/*
 * In maze::load_from_file std::copy + std::istream_iterator + back_inserter can't be used
   because every block of 'X' is treated as separated string but I would like to rather
   whole line per string. So std::getline.
*/

namespace core
{

maze::maze(std::shared_ptr<presentation::renderer> renderer_,
           std::shared_ptr<maze_loader> loader)
 : game_object(INT_MAX, INT_MAX),
   drawable(renderer_),
   m_loader(loader)
{
}

bool maze::is_field_filled(int column, int row) const
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

char maze::get_field(int column, int row) const
{
    std::lock_guard<std::mutex> lock(maze_mutex);

    assert((0 <= column) && (column < static_cast<int>(content.size()) ));
    assert((0 <= row) && (row < static_cast<int>(content[column].size()) ));
    return content[column][row];
}

std::string maze::get_chunk(int leftdown_x, int leftdown_y,
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

void maze::move_field(int column, int row, int new_column, int new_row)
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

void maze::reset_field(int column, int row)
{
    std::lock_guard<std::mutex> lock(maze_mutex);

    const int size = static_cast<int>(content.size());
    assert((0 <= column) && (column < size));
    assert((0 <= row) && (row < size));
    content[column][row] = ' ';
}

int maze::size()
{
    std::lock_guard<std::mutex> lock(maze_mutex);
    return content.size();
}

void maze::update_content()
{
    std::lock_guard<std::mutex> lock(maze_mutex);

    auto temporary_content = m_loader->load();
    logger_.log("maze: content was load. Content dump diff:");
    for (size_t i = 0; i < temporary_content.size(); i++)
    {
        if (temporary_content[i] != content[i])
            logger_.log("row %d: %s", i, temporary_content[i].c_str());
    }

    content = temporary_content;
}

void maze::verify()
{
    std::lock_guard<std::mutex> lock(maze_mutex);

    for (size_t i = 0; i < content.size(); i++)
    {
        assert(content[i].size() == 50);
    }
}

void maze::load()
{
    std::lock_guard<std::mutex> lock(maze_mutex);

    content = m_loader->load();
    logger_.log("maze: content was load. Content dump:");

    for (size_t i = 0; i < content.size(); i++)
        logger_.log("row %d: %s", i, content[i].c_str());

    if (renderer != nullptr)
        renderer->load_image_and_register("brick", "../../../data/brick.bmp");
}

void maze::tick(unsigned short)
{
}

void maze::load_image()
{
    assert(false); // use load to load all stuff including images
//    std::lock_guard<std::mutex> lock(maze_mutex);

//    renderer->load_image_and_register("brick", "../../../data/brick.bmp");
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

std::shared_ptr<presentation::renderer> maze::get_renderer() const
{
    return renderer;
}

}
