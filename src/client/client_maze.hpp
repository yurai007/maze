#ifndef CLIENT_MAZE_HPP
#define CLIENT_MAZE_HPP

#include "../common/abstract_maze.hpp"

namespace core
{

class client_maze : public abstract_maze, public drawable
{
public:
    client_maze(std::shared_ptr<presentation::renderer> renderer_,
                std::shared_ptr<core::maze_loader> loader,
                bool visible);

    void load_image() override;
    void draw() override;
    void load() override;

    std::shared_ptr<presentation::renderer> get_renderer() const;
private:
    const bool is_visible;
};

}

#endif // CLIENT_MAZE_HPP
