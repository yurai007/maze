#include <iostream>
#include <atomic>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/process.hpp>
#include <cassert>
#include <type_traits>
#include "pthread.h"

#include "../common/messages.hpp"
#include "../common/byte_buffer.hpp"
#include "../common/thread_safe_queue.hpp"
#include "../common/abstract_maze.hpp"


/* This is asynchronous client
 * write_some and read_some blocks.
 * sizeof is OK for messages which are POD-s (get_chunk) but for other is NOT OK.

 * Boost.Process is not part of boost:) Boost.Process is header only so linker will be happy.
   Library is quite old and has many incompatible versions. I use version 0.5 from process.zip from SO:
   http://stackoverflow.com/questions/1683665/where-is-boost-process

 * __attribute__((packed)) useful when padding is not as expected - fireball_triggered
*/

namespace server_sct
{

using boost::asio::ip::tcp;

using namespace boost::process;
using namespace boost::process::initializers;
using namespace networking;
using namespace core;

child *global_server_process = nullptr;

# define ext_assert(expr) \
    if (!(expr)) {        \
        assert(global_server_process != nullptr);\
        terminate(*global_server_process); \
        __assert_fail (__STRING(expr), __FILE__, __LINE__, __ASSERT_FUNCTION); } \

boost::asio::io_service io_service;
tcp::resolver resolver(io_service);
tcp::socket m_socket(io_service);

template<class T>
void serialize_and_send(const T &request_msg, unsigned short msg_size)
{
    serialization::byte_buffer serialized_msg;
    serialized_msg.put_unsigned_short(msg_size + 1);
    serialized_msg.put_char(request_msg.message_id());
    request_msg.serialize_to_buffer(serialized_msg);

    boost::system::error_code error;
    size_t send_bytes = m_socket.write_some(boost::asio::buffer(serialized_msg.m_byte_buffer,
                                                         serialized_msg.offset), error);
    ext_assert(!error);
    ext_assert(send_bytes == static_cast<size_t>(serialized_msg.offset));
}

template<class T>
char recv_and_deserialize(T &response_msg, unsigned expected_msg_size)
{
    serialization::byte_buffer serialized_msg;
    boost::system::error_code error;
    size_t recieved_bytes = m_socket.read_some(boost::asio::buffer(serialized_msg.m_byte_buffer),
                                               error);
    ext_assert(!error);
    ext_assert(recieved_bytes == expected_msg_size);

    unsigned short msg_size = serialized_msg.get_unsigned_short();
    (void)msg_size;
    char msg_type = serialized_msg.get_char();
    response_msg.deserialize_from_buffer(serialized_msg);
    return msg_type;
}

void test_abstract_maze_conversions()
{
    assert(abstract_maze::to_extended('P', 0) == 0);
    assert(abstract_maze::to_extended('P', 1) == (1<<3));
    assert(abstract_maze::to_extended('P', 123) == (123<<3));
    assert(abstract_maze::to_extended('P', (1<<13)-1) == ((1<<13)<<3)-(1<<3));

    assert(abstract_maze::to_extended('M', 0) == 10);
    assert(abstract_maze::to_extended('M', 1) == ((1<<6) | 10));
    assert(abstract_maze::to_extended('M', 123) == ((123<<6) | 10));
    assert(abstract_maze::to_extended('M', (1<<10)-1) == ((((1<<10)-1)<<6) | 10));

    assert(abstract_maze::to_extended('E', 0) == 1);
    assert(abstract_maze::to_extended('E', 1) == ((1<<3) | 1));
    assert(abstract_maze::to_extended('E', 123) == ((123<<3) | 1));

    char p = 'P', m = 'M', x = 'X', space = ' ', f = 'F';
    unsigned short id1 = 123, id2 = (1<<13)-1, id3 = (1<<10)-1, id4 = 0;
    assert(abstract_maze::to_normal(abstract_maze::to_extended(p, id1)) == std::tie(p, id1));
    assert(abstract_maze::to_normal(abstract_maze::to_extended(p, id2)) == std::tie(p, id2));
    assert(abstract_maze::to_normal(abstract_maze::to_extended(m, id1)) == std::tie(m, id1));
    assert(abstract_maze::to_normal(abstract_maze::to_extended(m, id3)) == std::tie(m, id3));

    assert(abstract_maze::to_normal(abstract_maze::to_extended(x, id4)) == std::tie(x, id4));
    assert(abstract_maze::to_normal(abstract_maze::to_extended(space, id4)) == std::tie(space, id4));
    assert(abstract_maze::to_normal(abstract_maze::to_extended(f, id1)) == std::tie(f, id1));
    assert(abstract_maze::to_normal(abstract_maze::to_extended(f, id2)) == std::tie(f, id2));

    char e = 'E';
    unsigned short id5 = 1;
    assert(abstract_maze::to_normal(((1<<3) | 1)) == std::tie(e, id5));
    assert(abstract_maze::to_normal(((123<<3) | 1)) == std::tie(e, id1));

    std::string str1 = "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE";
    unsigned short counter = 0;
    auto str2 = abstract_maze::to_extended(str1, [&counter](auto field){
        return (field == 'E')? counter++ :0;
    });
    auto str3 = abstract_maze::to_normal(str2);
    assert(str1 == str3);

    // TO DO: whole strs from test_get_chunk_response*
    std::cout << __FUNCTION__ << ":     ok\n";
}

void test_get_chunk_response1()
{
    static_assert(std::is_pod<messages::get_chunk>::value, "Sizeof on non-POD is not msg size");

    std::cout << "Running test_get_chunk_response1\n";
    messages::get_chunk request_msg = {0, 1, 1, 0};

    serialize_and_send(request_msg, sizeof(request_msg));
    std::cout << "Sent get_chunk request to server\n";

    messages::get_chunk_response response_msg;
    char msg_type = recv_and_deserialize(response_msg, 19);

    ext_assert((int)msg_type == messages::get_chunk_response::message_id());
    std::string normal_chunk = abstract_maze::to_normal(response_msg.content);
    ext_assert(normal_chunk == "X XX");
    std::cout << "Recieved get_chunk_response from server\n";
}

void test_get_chunk_response2()
{
    static_assert(std::is_pod<messages::get_chunk>::value, "Sizeof on non-POD is not msg size");

    std::cout << "Running test_get_chunk_response2\n";
    messages::get_chunk request_msg = {0, 1, 6, 0};

    serialize_and_send(request_msg, sizeof(request_msg));
    std::cout << "Sent get_chunk request to server\n";

    messages::get_chunk_response response_msg;
    char msg_type = recv_and_deserialize(response_msg, 21);

    ext_assert((int)msg_type == messages::get_chunk_response::message_id());
    std::string normal_chunk = abstract_maze::to_normal(response_msg.content);
    ext_assert(normal_chunk == "X      XXXXXX ");
    std::cout << "Recieved get_chunk_response from server\n";
}

void test_get_chunk_response3()
{
    static_assert(std::is_pod<messages::get_chunk>::value, "Sizeof on non-POD is not msg size");

    std::cout << "Running test_get_chunk_response3\n";
    messages::get_chunk request_msg = {0, 13, 19, 11};

    serialize_and_send(request_msg, sizeof(request_msg));
    std::cout << "Sent get_chunk request to server\n";

    messages::get_chunk_response response_msg;
    char msg_type = recv_and_deserialize(response_msg, 67);

    ext_assert((int)msg_type == messages::get_chunk_response::message_id());
    std::string normal_chunk = abstract_maze::to_normal(response_msg.content);
    ext_assert(normal_chunk == "      X  X   W     EXX       E      X   XX XXXXXXXXXXXXX   X");

    std::cout << "Recieved get_chunk_response from server\n";
}

//// get_enemies_data::player_id is not used on server side at all
//void test_get_enemies_data_response()
//{
//    static_assert(!std::is_pod<messages::get_enemies_data>::value, "Sizeof on non-POD is msg size");

//    std::cout << "Running test_get_enemies_data_response1\n";
//    messages::get_enemies_data request_msg(123);
//    unsigned short request_msg_size = request_msg.content.size() + sizeof(int);

//    serialize_and_send(request_msg, request_msg_size);
//    std::cout << "Sent get_enemies_data request to server\n";

//    messages::get_enemies_data_response response_msg;
//    char msg_type = recv_and_deserialize(response_msg, 715);

//    ext_assert((int)msg_type == messages::get_enemies_data_response::message_id());
//    std::vector<int> expected_enemies_data =
//    {1, 34, 0, 2, 26, 1, 3, 9, 2, 4, 42, 6, 5, 21, 7, 6, 12, 9, 7, 51, 9, 8, 35, 10, 9, 39, 10,
//    10, 41, 10, 11, 46, 10, 12, 56, 10, 13, 9, 11, 14, 17, 11, 15, 32, 11, 16, 2, 12, 17, 14, 12,
//    18, 53, 12, 19, 50, 13, 20, 2, 15, 21, 8, 15, 22, 29, 16, 23, 41, 18, 24, 11, 19, 25, 46, 19,
//    26, 29, 21, 27, 55, 21, 28, 32, 22, 29, 55, 24, 30, 6, 25, 31, 16, 25, 32, 10, 28, 33, 16, 28,
//    34, 27, 29, 35, 14, 30, 36, 53, 30, 37, 59, 33, 38, 38, 34, 39, 41, 34, 40, 20, 36, 41, 24, 36,
//    42, 5, 37, 43, 38, 42, 44, 42, 42, 45, 29, 43, 46, 32, 43, 47, 55, 47, 48, 1, 51, 49, 14, 51,
//    50, 16, 51, 51, 53, 51, 52, 23, 52, 53, 27, 52, 54, 32, 52, 55, 36, 52, 56, 41, 52, 57, 9, 53,
//    58, 44, 54, 59, 3, 57};

//    ext_assert(expected_enemies_data == response_msg.content);
//    std::cout << "Recieved get_enemies_data response from server\n";
//}

void test_get_id_response()
{
    static_assert(!std::is_pod<messages::get_id>::value, "Sizeof on non-POD is msg size");

    std::cout << "Running test_get_id_response\n";
    messages::get_id request_msg;
    unsigned short request_msg_size = request_msg.content.size();

    serialize_and_send(request_msg, request_msg_size);
    std::cout << "Sent get_id request to server\n";

    messages::get_id_response response_msg;
    char msg_type = recv_and_deserialize(response_msg, sizeof(response_msg) + 3);

    ext_assert((int)msg_type == messages::get_id_response::message_id());
    ext_assert(response_msg.player_id >= 0);
    std::cout << "Recieved get_id_response from server\n";
}

void test_get_id_response_and_client_shutdown()
{
    static_assert(!std::is_pod<messages::get_id>::value, "Sizeof on non-POD is msg size");

    std::cout << "Running tests_get_id_response_and_client_shutdown\n";
    messages::get_id request_msg1;
    unsigned short request_msg_size1 = request_msg1.content.size();

    serialize_and_send(request_msg1, request_msg_size1);
    std::cout << "Sent get_id request to server\n";

    messages::get_id_response response_msg1;
    char msg_type1 = recv_and_deserialize(response_msg1, sizeof(response_msg1) + 3);

    ext_assert((int)msg_type1 == messages::get_id_response::message_id());
    ext_assert(response_msg1.player_id >= 0);
    std::cout << "Recieved get_id_response from server\n";


    messages::client_shutdown request_msg2;
    unsigned short request_msg_size2 = sizeof(request_msg2);

    serialize_and_send(request_msg2, request_msg_size2);
    std::cout << "Sent client_shutdown request to server\n";

    messages::client_shutdown_response response_msg2;
    char msg_type2 = recv_and_deserialize(response_msg2, response_msg2.content.size() + 3);

    ext_assert((int)msg_type2 == messages::client_shutdown_response::message_id());
    ext_assert(response_msg2.content == "OK");
    std::cout << "Recieved client_shutdown_response from server\n";
}

void test_malformed_message__unknown_id_no_handler_in_game_server()
{
    static_assert(!std::is_pod<messages::get_id>::value, "Sizeof on non-POD is msg size");

    std::cout << "Running test_malformed_message__unknown_id_no_handler_in_game_server\n";
    messages::get_id request_msg;
    unsigned short msg_size = request_msg.content.size();


    serialization::byte_buffer serialized_msg;
    serialized_msg.put_unsigned_short(msg_size + 1);
    serialized_msg.put_char((char)(123));
    request_msg.serialize_to_buffer(serialized_msg);

    boost::system::error_code error;
    size_t send_bytes = m_socket.write_some(boost::asio::buffer(serialized_msg.m_byte_buffer,
                                                         serialized_msg.offset), error);
    ext_assert(!error);
    ext_assert(send_bytes == static_cast<size_t>(serialized_msg.offset));

    std::cout << "Sent get_id request to server\n";

    messages::internal_error_message response_msg;
    char msg_type = recv_and_deserialize(response_msg, 10);

    ext_assert((int)msg_type == messages::internal_error_message::message_id());
    ext_assert(response_msg.content == "NOK");
    std::cout << "Recieved internal_error_message from server\n";
}

void test_bad_message__handler_in_game_server_throws()
{
    static_assert(std::is_pod<messages::fireball_triggered>::value, "Sizeof on non-POD is msg size");

    std::cout << "Running test_bad_message__handler_in_game_server_throws\n";
    messages::fireball_triggered request_msg = {1, 0, 0, '?'};

    serialize_and_send(request_msg, sizeof(request_msg));
    std::cout << "Sent fireball_triggered request to server\n";

    messages::internal_error_message response_msg;
    char msg_type = recv_and_deserialize(response_msg, 10);

    ext_assert((int)msg_type == messages::internal_error_message::message_id());
    ext_assert(response_msg.content == "NOK");
    std::cout << "Recieved internal_error_message from server\n";
}

void connect_handler(const boost::system::error_code &error_code)
{
    if (!error_code)
    {
//      TO DO: maze content changed so I must alignt those tests

//      test_get_chunk_response1();
//      test_get_chunk_response2();
//      test_get_chunk_response3();
//      test_get_enemies_data_response();

//      TO DO: sth wrong here, fix this test !
//      test_get_id_response_and_client_shutdown();

        test_get_id_response();
        test_malformed_message__unknown_id_no_handler_in_game_server();
        test_bad_message__handler_in_game_server_throws();

        std::cout << "All server SCT passed\n";
    }
    else
    {
        std::cout << "Connection failed\n";
        ext_assert(false);
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
    test_abstract_maze_conversions();

    try
    {
        std::cout << "Running sct_server tests...\n";

        auto server_process = execute(
                    run_exe("maze_server"),
                    set_cmd_line("../server/maze_server pause-for-test"),
                    start_in_dir("../server/")
                    );
        global_server_process = &server_process;
        sleep(1);

        tcp::resolver::query query(tcp::tcp::v4(), "127.0.0.1", "5555");

        resolver.async_resolve(query, boost::bind( &resolve_handler,
             boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred) );

         // blocks until all work will be done (So until all handlers will finish)
        io_service.run();

        terminate(server_process);
        global_server_process = nullptr;
    }
    catch (std::exception& e)
    {
        std::cout << "Problem!\n";
        std::cerr << e.what() << std::endl;
    }
}

}
