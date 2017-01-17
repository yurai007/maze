#ifndef ABSTRACT_MAZE_HPP
#define ABSTRACT_MAZE_HPP

#include <string>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <cstring>

#include "game_object.hpp"
#include "maze_loader.hpp"
#include "smart_ptr.hpp"

namespace core
{

class abstract_maze : public game_object
{
public:
    abstract_maze(smart::fit_smart_ptr<core::maze_loader> loader);

    bool is_field_filled(int column, int row) const;
    char get_field(int column, int row) const;
    unsigned short get_id(int column, int row) const;
    void set_field(int column, int row, char field);

    std::string get_chunk(unsigned leftdown_x, unsigned leftdown_y,
                                       unsigned rightupper_x, unsigned rightupper_y) const;

    void move_field(const std::tuple<int, int> old_pos,
                    const std::tuple<int, int> new_pos);
    void reset_field(const std::tuple<int, int> pos);
    int size() const;
    size_t column_size(int column) const;
    void update_content();
    void verify() const;
    virtual ~abstract_maze() = default;

    void tick(unsigned short) override;

    static unsigned short to_extended(char field, unsigned short id)
    {
        assert(id < (1<<13));
        assert(field == 'P' || field == 'E' || field == 'G' || field == 'W' || field == 'M'
               || field == 's' || field == 'S' || field == 'X' || field == ' ' || field == 'F');

        static std::unordered_map<char, char> to_type =
        {
            {'P', 0x0}, {'E', 0x1},
            {'G', 0x0 << 3 | 0x2}, {'M', 0x1 << 3 | 0x2}, {'S', 0x2 << 3 | 0x2},
            {'W', 0x3 << 3 | 0x2}, {'s', 0x4 << 3 | 0x2},
            {'X', 0x3}, {' ', 0x4}, {'F', 0x5}
        };
        unsigned char type = to_type[field];
        unsigned short value = ((type & 0x7) == 0x2)? (type | id << 6) : (type | id << 3);
        return value;
    }

    static std::string to_extended(const std::string &str,
                                   std::function<unsigned short(char)> enumerator)
    {
        std::string result(2*str.size(), ' ');
        for (unsigned i = 0; i < str.size(); i++)
        {
            auto value = to_extended(str[i], enumerator(str[i]));
            memcpy(reinterpret_cast<char*>(&result[2*i]), reinterpret_cast<char*>(&value), 2);
        }
        return result;
    }

    static std::tuple<char, unsigned short> to_normal(unsigned short org_field)
    {
        static std::unordered_map<char, char> from_type =
        {
            {0x0, 'P'}, {0x1, 'E'},
            {0x0 << 3 | 0x2, 'G'}, {0x1 << 3 | 0x2, 'M'}, {0x2 << 3 | 0x2, 'S'},
            {0x3 << 3 | 0x2, 'W'}, {0x4 << 3 | 0x2, 's'},
            {0x3, 'X'}, {0x4, ' '}, {0x5, 'F'}
        };

        unsigned char type = 0;
        unsigned short id = 0;
        if ((org_field & 0x7) == 0x2)
        {
            type = org_field & 0x3F;
            id = org_field >> 6;
        }
        else
        {
            type = org_field & 0x7;
            id = org_field >> 3;
        }

        assert(id < (1<<13));
        char field = from_type[type];
        assert(field == 'P' || field == 'E' || field == 'G' || field == 'W' || field == 'M'
               || field == 's' || field == 'S' || field == 'X' || field == ' ' || field == 'F');
        return {field, id};
    }

    static std::string to_normal(const std::string &str)
    {
        std::string result(str.size()/2, ' ');
        for (unsigned i = 0; i < result.size(); i++)
        {
            unsigned short value = 0, id = 0;
            memcpy(&value, reinterpret_cast<char*>(const_cast<char*>(&str[2*i])), 2);
            std::tie(result[i], id) = to_normal(value);
        }
        return result;
    }

protected:
    unsigned short get_extended_field(int column, int row) const;
    void set_extended_field(int column, int row, unsigned short field);

    std::vector<std::string> content;
    smart::fit_smart_ptr<core::maze_loader> m_loader {nullptr};
};

}

#endif // ABSTRACT_MAZE_HPP
