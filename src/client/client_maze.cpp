#include <cassert>
#include "client_maze.hpp"
#include "renderer.hpp"

namespace core
{

client_maze::client_maze(std::shared_ptr<presentation::renderer> renderer_,
                         std::shared_ptr<maze_loader> loader)
    : abstract_maze(loader),
      drawable(renderer_)
{
}

void client_maze::load_image()
{
    assert(false);
}

void client_maze::draw()
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

void client_maze::load()
{
    std::lock_guard<std::mutex> lock(maze_mutex);
    assert(renderer != nullptr);

    content = m_loader->load();
    logger_.log("client_maze: content was load. Content dump:");

    for (size_t i = 0; i < content.size(); i++)
        logger_.log("row %d: %s", i, content[i].c_str());

    renderer->load_image_and_register("brick", "../../../data/brick.bmp");
}

std::shared_ptr<presentation::renderer> client_maze::get_renderer() const
{
    return renderer;
}

}
