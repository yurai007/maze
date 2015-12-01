#ifndef CLIENT_MAZE_HPP
#define CLIENT_MAZE_HPP

#include "../common/abstract_maze.hpp"
#include "client_player.hpp"

namespace core
{

class client_maze : public abstract_maze, public drawable
{
public:
    client_maze(std::shared_ptr<presentation::renderer> renderer_,
                std::shared_ptr<core::maze_loader> loader,
                bool visible);

    void load_image() override;
    void draw(int active_player_x, int active_player_y) override;
    void load() override;
    void attach_active_player(std::shared_ptr<client_player> player);

    std::shared_ptr<presentation::renderer> get_renderer() const;
private:
    const bool is_visible;
    std::shared_ptr<client_player> active_player {nullptr};
};

}

#endif // CLIENT_MAZE_HPP
