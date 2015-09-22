#ifndef MAZE_HPP
#define MAZE_HPP

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include "game_object.hpp"
#include "maze_loader.hpp"

namespace core
{

class maze : public game_object, public drawable
{
public:
    maze(std::shared_ptr<presentation::renderer> renderer_,
         std::shared_ptr<core::maze_loader> loader);

    bool is_field_filled(int column, int row) const;
    char get_field(int column, int row) const;

    std::string get_chunk(int leftdown_x, int leftdown_y,
                                       int rightupper_x, int rightupper_y) const;

    void move_field(int column, int row, int new_column, int new_row);
    void reset_field(int column, int row);
    int size();
    void update_content();
    void verify();
    void load();
    std::shared_ptr<presentation::renderer> get_renderer() const;

    void tick(unsigned short) override;

    void load_image() override;
    void draw() override;

private:
    std::vector<std::string> content;
    mutable std::mutex maze_mutex;
    std::shared_ptr<core::maze_loader> m_loader;
};

}

#endif // MAZE_HPP
