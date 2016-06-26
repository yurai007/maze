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


/* This is asynchronous client
 * write_some and read_some blocks.
 * sizeof is OK for messages which are POD-s (get_chunk) but for other is NOT OK.

 * Boost process is not part of boost:) Boost Process is header only so linker will be happy.
   Library is quite old and has many incompatible versions. I use version 0.5 from process.zip from SO:
   http://stackoverflow.com/questions/1683665/where-is-boost-process

   TO DO: pause/resume for server, fix test_get_chunk_response3 and add tests for position_changed
*/

namespace server_sct
{

using boost::asio::ip::tcp;

using namespace boost::process;
using namespace boost::process::initializers;
using namespace networking;

child *global_server_process = nullptr;

# define ext_assert(expr) \
    if (!(expr)) {        \
        assert(global_server_process != nullptr);\
        terminate(*global_server_process); \
        __assert_fail (__STRING(expr), __FILE__, __LINE__, __ASSERT_FUNCTION); } \

boost::asio::io_service io_service;
tcp::resolver resolver(io_service);
tcp::socket m_socket(io_service);

void test_get_chunk_response1()
{
    static_assert(std::is_pod<messages::get_chunk>::value, "Sizeof on non-POD is not msg size");

    std::cout << "Running test_get_chunk_response1\n";
    messages::get_chunk request_msg = {0, 1, 1, 0};

    serialization::byte_buffer serialized_msg;
    unsigned short msg_size = sizeof(request_msg) + 1;
    serialized_msg.put_unsigned_short(msg_size);
    serialized_msg.put_char(request_msg.message_id());
    request_msg.serialize_to_buffer(serialized_msg);

    boost::system::error_code error;
    size_t send_bytes = m_socket.write_some(boost::asio::buffer(serialized_msg.m_byte_buffer,
                                                         serialized_msg.offset), error);
    ext_assert(!error);
    ext_assert(send_bytes == 19);
    std::cout << "Sent get_chunk request to server\n";

    serialized_msg.clear();
    size_t recieved_bytes = m_socket.read_some(boost::asio::buffer(serialized_msg.m_byte_buffer),
                                               error);
    ext_assert(!error);
    ext_assert(recieved_bytes == 11);

    msg_size = serialized_msg.get_unsigned_short();
    char msg_type = serialized_msg.get_char();
    messages::get_chunk_response response_msg;
    response_msg.deserialize_from_buffer(serialized_msg);

    ext_assert((int)msg_type == 1);
    ext_assert(response_msg.content == "X XX");
    std::cout << "Recieved get_chunk_response from server\n";
}

void test_get_chunk_response2()
{
    static_assert(std::is_pod<messages::get_chunk>::value, "Sizeof on non-POD is not msg size");

    std::cout << "Running test_get_chunk_response2\n";
    messages::get_chunk request_msg = {0, 1, 6, 0};
    serialization::byte_buffer serialized_msg;
    unsigned short msg_size = sizeof(request_msg) + 1;
    serialized_msg.put_unsigned_short(msg_size);
    serialized_msg.put_char(request_msg.message_id());
    request_msg.serialize_to_buffer(serialized_msg);

    boost::system::error_code error;
    size_t send_bytes = m_socket.write_some(boost::asio::buffer(serialized_msg.m_byte_buffer,
                                                         serialized_msg.offset), error);
    ext_assert(!error);
    ext_assert(send_bytes == 19);
    std::cout << "Sent get_chunk request to server\n";

    serialized_msg.clear();
    size_t recieved_bytes = m_socket.read_some(boost::asio::buffer(serialized_msg.m_byte_buffer),
                                               error);
    ext_assert(!error);
    ext_assert(recieved_bytes == 21);

    msg_size = serialized_msg.get_unsigned_short();
    char msg_type = serialized_msg.get_char();
    messages::get_chunk_response response_msg;
    response_msg.deserialize_from_buffer(serialized_msg);

    ext_assert((int)msg_type == 1);
    ext_assert(response_msg.content == "X      XXXXXX ");
    std::cout << "Recieved get_chunk_response from server\n";
}

// sometimes fail because of enemies movement
void test_get_chunk_response3()
{
    static_assert(std::is_pod<messages::get_chunk>::value, "Sizeof on non-POD is not msg size");

    std::cout << "Running test_get_chunk_response3\n";
    messages::get_chunk request_msg = {0, 13, 19, 11};
    serialization::byte_buffer serialized_msg;
    unsigned short msg_size = sizeof(request_msg) + 1;
    serialized_msg.put_unsigned_short(msg_size);
    serialized_msg.put_char(request_msg.message_id());
    request_msg.serialize_to_buffer(serialized_msg);

    boost::system::error_code error;
    size_t send_bytes = m_socket.write_some(boost::asio::buffer(serialized_msg.m_byte_buffer,
                                                         serialized_msg.offset), error);
    ext_assert(!error);
    ext_assert(send_bytes == 19);
    std::cout << "Sent get_chunk request to server\n";

    serialized_msg.clear();
    size_t recieved_bytes = m_socket.read_some(boost::asio::buffer(serialized_msg.m_byte_buffer),
                                               error);
    ext_assert(!error);
    ext_assert(recieved_bytes == 67);

    msg_size = serialized_msg.get_unsigned_short();
    char msg_type = serialized_msg.get_char();
    messages::get_chunk_response response_msg;
    response_msg.deserialize_from_buffer(serialized_msg);

    ext_assert((int)msg_type == 1);
    // TO DO
    //ext_assert(response_msg.content == "      X  X   W     EXX       E      X   XX XXXXXXXXXXXXX   X");

    std::cout << "Recieved get_chunk_response from server\n";
}

void connect_handler(const boost::system::error_code &error_code)
{
    if (!error_code)
    {
        test_get_chunk_response1();
        test_get_chunk_response2();
        test_get_chunk_response3();

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
    try
    {
        std::cout << "Running sct_server tests...\n";

        auto server_process = execute(
                    run_exe("maze_server"),
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


