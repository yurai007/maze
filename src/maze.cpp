#include "maze.hpp"
#include <fstream>
#include <iterator>
#include <cassert>

#include "renderer.hpp"

/*
 * In maze::load_from_file std::copy + std::istream_iterator + back_inserter can't be used
   because every block of 'X' is treated as separeted string but I would like to rather
   whole line per string. So std::getline.
*/

namespace core
{

maze::maze(presentation::renderer *renderer_)
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

    renderer->load_image_and_register("brick", "../../data/brick.bmp");
}

bool maze::is_field_filled(int column, int row)
{
    assert((0 <= column) && (column < content.size()));
    assert((0 <= row) && (row < content.size()));
    return content[column][row] == 'X';
}

int maze::size()
{
    return content.size();
}

void maze::tick()
{
}

void maze::draw()
{
    int posx = 0, posy = 0;
    const int brick_size = 30;
    for (int row = 0; row < size(); row++)
        for (int column = 0; column < size(); column++)
        {
            if (is_field_filled(column, row))
            {
                posx = column * brick_size;
                posy = row * brick_size;

                renderer->draw_image("brick", posx, posy);
            }
        }
}

}
