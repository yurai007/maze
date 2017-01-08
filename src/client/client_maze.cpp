#include <cassert>
#include "client_maze.hpp"
#include "renderer.hpp"

namespace core
{

client_maze::client_maze(smart::fit_smart_ptr<presentation::renderer> renderer_,
                         smart::fit_smart_ptr<maze_loader> loader,
                         bool visible)
    : abstract_maze(loader),
      drawable(renderer_),
      is_visible(visible)
{
    assert(m_loader != nullptr);
}

void client_maze::load_image()
{
    assert(false);
}

void client_maze::draw(int active_player_x, int active_player_y)
{
    (void)active_player_x; (void)active_player_y;

    if (!is_visible)
        return;
    if (active_player == nullptr)
        return;

    auto pos = active_player->get_position();
    const int x = std::get<0>(pos), y = std::get<1>(pos);

    const int half_width = 50/2;
    const int half_height = 50/2;
    const int brick_size = 30;

    for (int row = y - half_height; row < y + half_height; row++)
        for (int column = x - half_width; column < x + half_width; column++)
        {
            if (0 <= column && column < static_cast<int>(content.size()))
                if (0 <= row && row < static_cast<int>(column_size(column)))
                    if (is_field_filled(column, row))
                    {
                        const int posx = (column - (x - half_width)) * brick_size;
                        const int posy = (row - (y - half_height)) * brick_size;

                        renderer->draw_image("brick", posx, posy);
                    }
        }
}

void client_maze::load()
{
    content = m_loader->load();
    logger_.log("client_maze: content was load");
    logger_.log("client_maze: content: ");

    for (size_t i = 0; i < content.size(); i++)
        logger_.log("row %d: %s", i, content[i].c_str());

    if (is_visible)
    {
        assert(renderer != nullptr);
        renderer->load_image_and_register("brick", "../../../data/brick.bmp");
    }
}

void client_maze::attach_active_player(smart::fit_smart_ptr<client_player> player)
{
    active_player = player;
}

smart::fit_smart_ptr<presentation::renderer> client_maze::get_renderer() const
{
    return renderer;
}

}
