#include <iostream>
#include <atomic>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <cassert>
#include <type_traits>
#include "pthread.h"

#include "../common/messages.hpp"
#include "../common/byte_buffer.hpp"
#include "../common/thread_safe_queue.hpp"

namespace byte_buffer_ut
{

using namespace serialization;


// ref: http://www.cplusplus.com/reference/type_traits/is_integral/
// platform dependent!
// http://stackoverflow.com/questions/4603717/stopping-function-implicit-conversion
void test_case_integral_types()
{
    byte_buffer buffer;
    buffer.put_bool(true);
    buffer.put_char('c');
    buffer.put_int(123456);
    buffer.put_long(987654321L);

    size_t expected_offset = sizeof(bool) + sizeof(char) + sizeof(int) + sizeof(long);
    assert(buffer.get_size() == expected_offset);
    buffer.set_offset_on_start();
    assert(buffer.get_size() == 0);
    assert(buffer.get_bool() == true);
    assert(buffer.get_char() == 'c');
    assert(buffer.get_int() == 123456);
    assert(buffer.get_long() == 987654321L);
    assert(buffer.get_size() == expected_offset);
}

void test_case_non_integral_types()
{
    byte_buffer buffer;
    std::string tested_string = "Hejka ha;)";
    std::vector<double> tested_doubles = { 1.1, 1.2, 1.3 };
    std::vector<int> tested_ints = {1, 2, 3};
    std::vector<long> tested_longs = {987654321L};
    std::vector<int> tested_ints2 = {};

    buffer.put_string(tested_string);
    buffer.put_double_vector(tested_doubles);
    buffer.put_int_vector(tested_ints);
    buffer.put_long_vector(tested_longs);
    buffer.put_int_vector(tested_ints2);

    size_t expected_offset = tested_string.size() + sizeof(int) +
            sizeof(double)*tested_doubles.size() + sizeof(int) +
            sizeof(int)*tested_ints.size() + sizeof(int) +
            sizeof(long)*tested_longs.size() + sizeof(int) +
            sizeof(int)*tested_ints2.size() + sizeof(int);

    assert(buffer.get_size() == expected_offset);
    buffer.set_offset_on_start();
    assert(buffer.get_size() == 0);
    assert(buffer.get_string() == "Hejka ha;)");
    assert(buffer.get_double_vector() == tested_doubles);
    assert(buffer.get_int_vector() == tested_ints);
    assert(buffer.get_long_vector() == tested_longs);
    assert(buffer.get_int_vector() == tested_ints2);
    assert(buffer.get_size() == expected_offset);
}

void test_cases()
{
    test_case_integral_types();
    test_case_non_integral_types();
    std::cout << "All byte_buffer UT passed\n";
}
}

