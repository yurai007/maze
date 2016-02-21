#ifndef ABSTRACT_MAZE_HPP
#define ABSTRACT_MAZE_HPP

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include "game_object.hpp"
#include "maze_loader.hpp"

namespace core
{

class abstract_maze : public game_object
{
public:
    abstract_maze(std::shared_ptr<core::maze_loader> loader);

    bool is_field_filled(int column, int row) const;
    char get_field(int column, int row) const;
    void set_field(int column, int row, char field);

    std::string get_chunk(unsigned leftdown_x, unsigned leftdown_y,
                                       unsigned rightupper_x, unsigned rightupper_y) const;

    void move_field(const std::tuple<int, int> old_pos,
                    const std::tuple<int, int> new_pos);
    void reset_field(const std::tuple<int, int> pos);
    int size() const;
    void update_content();
    void verify() const;
    virtual void load() = 0;
    virtual ~abstract_maze() = default;

    void tick(unsigned short) override;

protected:
    std::vector<std::string> content;
    mutable std::mutex maze_mutex;
    std::shared_ptr<core::maze_loader> m_loader;
};

}

#endif // ABSTRACT_MAZE_HPP
