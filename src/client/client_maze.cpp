#include <cassert>
#include "client_maze.hpp"
#include "renderer.hpp"

namespace core
{

client_maze::client_maze(std::shared_ptr<presentation::renderer> renderer_,
                         std::shared_ptr<maze_loader> loader, bool visible)
    : abstract_maze(loader),
      drawable(renderer_),
      is_visible(visible)
{
}

void client_maze::load_image()
{
    assert(false);
}

void client_maze::draw(int active_player_x, int active_player_y)
{
    if (!is_visible)
        return;
    if (!active_player)
        return;

    auto pos = active_player->get_position();
    const int x = std::get<0>(pos), y = std::get<1>(pos);

    const int half_width = 50/2;
    const int half_height = 50/2;
    const int brick_size = 30;

    for (int row = y - half_height; row < y + half_height; row++)
        for (int column = x - half_width; column < x + half_width; column++)
        {
            if (is_field_filled(column, row))
            {
                const int posx = (column - (x - half_width)) * brick_size;
                const int posy = (row - (y - half_height)) * brick_size;

                std::lock_guard<std::mutex> lock(maze_mutex);
                renderer->draw_image("brick", posx, posy);
            }
        }
}

void client_maze::load()
{
    if (!is_visible)
    {
        std::lock_guard<std::mutex> lock(maze_mutex);
        content = m_loader->load();
        logger_.log("client_maze: content was load");
        logger_.log_debug("client_maze: content: ");

        for (size_t i = 0; i < content.size(); i++)
            logger_.log_debug("row %d: %s", i, content[i].c_str());
        return;
    }

    std::lock_guard<std::mutex> lock(maze_mutex);
    assert(renderer != nullptr);

    content = m_loader->load();
    logger_.log("client_maze: content was load");
    logger_.log_debug("client_maze: content: ");

    for (size_t i = 0; i < content.size(); i++)
        logger_.log_debug("row %d: %s", i, content[i].c_str());

    renderer->load_image_and_register("brick", "../../../data/brick.bmp");
}

void client_maze::attach_active_player(std::shared_ptr<client_player> player)
{
    active_player = player;
}

std::shared_ptr<presentation::renderer> client_maze::get_renderer() const
{
    return renderer;
}

}
