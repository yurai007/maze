#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <cassert>
#include <type_traits>

#include "../common/messages.hpp"
#include "../common/byte_buffer.hpp"

/* This is asynchronous client
 * write_some and read_some blocks.
 * sizeof is OK for messages which are POD-s (get_chunk) but for
   other is NOT OK.
 *  TO DO: pause/resume for server, fix test_get_chunk_response3 and add tests for position_changed
*/

namespace server_sct
{

using boost::asio::ip::tcp;
using namespace networking;

boost::asio::io_service io_service;
tcp::resolver resolver(io_service);
tcp::socket m_socket(io_service);

void test_get_chunk_response1()
{
    static_assert(std::is_pod<messages::get_chunk>::value, "Sizeof on non-POD is not msg size");

    std::cout << "[test_get_chunk_response1]\n";
    messages::get_chunk msg = {0, 1, 1, 0};

    serialization::byte_buffer serialized_msg;
    char msg_size = sizeof(msg) + 1;
    serialized_msg.put_char(msg_size);
    serialized_msg.put_char(msg.message_id());
    msg.serialize_to_buffer(serialized_msg);

    boost::system::error_code error;
    size_t send_bytes = m_socket.write_some(boost::asio::buffer(serialized_msg.m_byte_buffer,
                                                         serialized_msg.offset), error);
    assert(!error);
    assert(send_bytes == 18);
    std::cout << "Send get_chunk to server\n";

    serialized_msg.clear();
    size_t recieved_bytes = m_socket.read_some(boost::asio::buffer(serialized_msg.m_byte_buffer),
                                               error);
    assert(!error);
    assert(recieved_bytes == 10);

    msg_size = serialized_msg.get_char();
    char msg_type = serialized_msg.get_char();
    messages::get_chunk_response msg2;
    msg2.deserialize_from_buffer(serialized_msg);

    assert((int)msg_type == 1);
    assert(msg2.content == "  XX");
    std::cout << "Recieved get_chunk_response from server\n";
}

void test_get_chunk_response2()
{
    static_assert(std::is_pod<messages::get_chunk>::value, "Sizeof on non-POD is not msg size");

    std::cout << "[test_get_chunk_response2]\n";
    messages::get_chunk msg = {0, 1, 6, 0};
    serialization::byte_buffer serialized_msg;
    char msg_size = sizeof(msg) + 1;
    serialized_msg.put_char(msg_size);
    serialized_msg.put_char(msg.message_id());
    msg.serialize_to_buffer(serialized_msg);

    boost::system::error_code error;
    size_t send_bytes = m_socket.write_some(boost::asio::buffer(serialized_msg.m_byte_buffer,
                                                         serialized_msg.offset), error);
    assert(!error);
    assert(send_bytes == 18);
    std::cout << "Send get_chunk to server\n";

    serialized_msg.clear();
    size_t recieved_bytes = m_socket.read_some(boost::asio::buffer(serialized_msg.m_byte_buffer),
                                               error);
    assert(!error);
    assert(recieved_bytes == 20);

    msg_size = serialized_msg.get_char();
    char msg_type = serialized_msg.get_char();
    messages::get_chunk_response msg2;
    msg2.deserialize_from_buffer(serialized_msg);

    assert((int)msg_type == 1);
    assert(msg2.content == "       XXXXXXX");
    std::cout << "Recieved get_chunk_response from server\n";
}

// sometimes fail because of enemies movement
void test_get_chunk_response3()
{
    static_assert(std::is_pod<messages::get_chunk>::value, "Sizeof on non-POD is not msg size");

    std::cout << "[test_get_chunk_response3]\n";
    messages::get_chunk msg = {0, 13, 19, 11};
    serialization::byte_buffer serialized_msg;
    char msg_size = sizeof(msg) + 1;
    serialized_msg.put_char(msg_size);
    serialized_msg.put_char(msg.message_id());
    msg.serialize_to_buffer(serialized_msg);

    boost::system::error_code error;
    size_t send_bytes = m_socket.write_some(boost::asio::buffer(serialized_msg.m_byte_buffer,
                                                         serialized_msg.offset), error);
    assert(!error);
    assert(send_bytes == 18);
    std::cout << "Send get_chunk to server\n";

    serialized_msg.clear();
    size_t recieved_bytes = m_socket.read_some(boost::asio::buffer(serialized_msg.m_byte_buffer),
                                               error);
    assert(!error);
    assert(recieved_bytes == 66);

    msg_size = serialized_msg.get_char();
    char msg_type = serialized_msg.get_char();
    messages::get_chunk_response msg2;
    msg2.deserialize_from_buffer(serialized_msg);

    assert((int)msg_type == 1);
    assert(msg2.content == "      X  X          XX              X    X XXXXXXXXXXXXX   X");
    std::cout << "Recieved get_chunk_response from server\n";
}

void connect_handler(const boost::system::error_code &error_code)
{
    if (!error_code)
    {
        test_get_chunk_response1();
        test_get_chunk_response2();
        test_get_chunk_response3();

        std::cout << "All tests passed\n";
    }
    else
    {
        std::cout << "Connection failed\n";
        assert(false);
    }
}

void resolve_handler(const boost::system::error_code &error_code,
                     tcp::resolver::iterator endpoint_iterator)
{
    if (!error_code)
    {
        m_socket.async_connect(*endpoint_iterator, connect_handler);
    }
    else
    {
        std::cout << "Resolving failed, error: " << error_code.value() << "\n";
        exit(1);
    }
}

void test_cases()
{
    try
    {
        tcp::resolver::query query(tcp::tcp::v4(), "127.0.0.1", "5555");

        resolver.async_resolve(query, boost::bind( &resolve_handler,
             boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred) );

         // blocks until all work will be done (So until all handlers will finish)
        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cout << "Problem!\n";
        std::cerr << e.what() << std::endl;
    }
}

}

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
    std::cout << "All tests passed\n";
}
}

int main(int, char*[])
{
    byte_buffer_ut::test_cases();
    server_sct::test_cases();
    return 0;
}
