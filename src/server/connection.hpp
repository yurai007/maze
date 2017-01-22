#ifndef CONNECTION_H
#define CONNECTION_H

#include <boost/asio.hpp>

#include "../common/logger.hpp"
#include "../common/byte_buffer.hpp"

namespace networking
{

using namespace boost::asio;
using ip::tcp;

class server;

class connected_socket
{
public:
    connected_socket(tcp::socket socket)
        : _socket(std::move(socket)) {}

    tcp::socket _socket;

    void close()
    {
        _socket.close();
    }

    int native_handle()
    {
        return _socket.native_handle();
    }

    using error_type = boost::system::error_code;
};

class input_stream
{
public:
    template<class Func>
    void read_at_least_one_byte(connected_socket &socket, void* dest, size_t bytes,
                                Func func) {
        socket._socket.async_read_some(buffer(dest, bytes), func);
    }
};

class output_stream
{
public:
    template<class Func>
    void write_all(connected_socket &socket, void* dest, size_t bytes,
                   Func func) {
        async_write(socket._socket, buffer(dest, bytes), func);
    }
};

class connection
{
public:
    connection(const connection&) = delete;
    connection& operator=(const connection&) = delete;

    connection(connected_socket socket_, server &server_);
    connected_socket &get_socket();
    void start();
    void stop();
    unsigned id {0};

private:

    void process(const connected_socket::error_type& error, size_t bytes_transferred);

    server &m_server;
    connected_socket socket;
    input_stream read_buf;
    output_stream write_buf;

    serialization::byte_buffer data_buffer;
    int remaining_bytes {0};
    size_t read_so_far_bytes {0};
};

}


#endif // CONNECTION_H
