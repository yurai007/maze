#include "maze_generator.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <sstream>

#include <array>

/*
 * size_t for values for size() or for values comparing with size()
 * ifstream and ofstream from <fstream>
 * ostream_iterator from <iterator>

 * how does std::copy is implemented and why? Look at stl_algobase.h...

 * problem occurs in stream_iterator.h for *_M_stream << __value; :

   ostream_iterator&
   operator=(const _Tp& __value)
    {
        __glibcxx_requires_cond(_M_stream != 0,
                                _M_message(__gnu_debug::__msg_output_ostream)
                                ._M_iterator(*this));
        *_M_stream << __value;
        if (_M_string) *_M_stream << _M_string;
        return *this;
    }

    Compiler msg:
    /usr/include/c++/4.9/bits/stream_iterator.h:198: error: cannot bind
    ‘std::ostream_iterator<std::vector<char> >::ostream_type {aka std::basic_ostream<char>}’ lvalue
    to ‘std::basic_ostream<char>&&’
         *_M_stream << __value;
                    ^
    So there is no operator<<(std::vector<char> ) I guess.
*/

namespace utils
{

maze_generator::maze_generator(int size)
{
    content.assign(size, std::string(size, ' '));
    srand(time(NULL));
}

void maze_generator::generate_with_patterns_from(const std::string &file_name, int d)
{
    std::ifstream input_file(file_name.c_str());
    if (!input_file.is_open())
        return;
    int patterns_number;


    std::string line;
    std::getline(input_file, line);
    std::istringstream helper_stream(line);
    helper_stream >> patterns_number;

    patterns.assign(patterns_number, std::vector<std::string>());
        for (int i = 0; i < patterns_number; i++)
            read_pattern(i, input_file);

    input_file.close();

    for (size_t i = d/2; i < content.size(); i += d)
        for (size_t j = d/2; j < content.size(); j += d)
        {
            const size_t pattern_id = get_pattern_id();
            apply_pattern_to_content(patterns[pattern_id], i, j);
        }
}

void maze_generator::save_to_file(const std::string &file_name)
{
    std::ofstream output_file(file_name);
    std::ostream_iterator<std::string> output_iterator(output_file, "\n");
    std::copy(content.begin(), content.end(), output_iterator);
    output_file.close();
}

void maze_generator::read_pattern(int i, std::ifstream &input_file)
{
    std::string line;
    while (std::getline(input_file, line))
    {
        const std::size_t pos = line.find("X");
        if (pos == std::string::npos)
            break;
        else
            patterns[i].push_back(line);
    }
}

void maze_generator::apply_pattern_to_content(const std::vector<std::string> &pattern,
                                              int column,
                                              int row)
{
    assert(!pattern.empty());

    const size_t row_number = pattern.size();
    const size_t column_number = pattern[0].size();
    const int first_row = row - row_number/2;
    const int first_column = column - column_number/2;

    for (size_t j = 0; j < row_number; j++)
    {
        for (size_t i = 0; i < pattern[j].size(); i++)
            if ( (pattern[j][i] == 'X') && (first_row + j < content.size())
                    && (first_column + i < content.size()) )
                content[first_row + j][first_column + i] = 'X';

    }
}

size_t maze_generator::get_pattern_id() const
{
    return rand()%patterns.size();
}

}


