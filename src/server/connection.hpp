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

class input_stream
{
public:
    template<class Func>
    void read_at_least_one_byte(tcp::socket &socket, void* dest, size_t bytes,
                                Func func) {
        socket.async_read_some(buffer(dest, bytes), func);
    }
};

class output_stream
{
public:
    template<class Func>
    void write_all(tcp::socket &socket, void* dest, size_t bytes,
                   Func func) {
        async_write(socket, buffer(dest, bytes), func);
    }
};

using error_type = boost::system::error_code;

class connection
{
public:
    connection(const connection&) = delete;
    connection& operator=(const connection&) = delete;

    connection(io_service& io_service, server &server_);
    tcp::socket& get_socket();
    void start();
    void stop();
    unsigned id {0};

private:

    void process(const error_type& error, size_t bytes_transferred);

    server &m_server;
    tcp::socket socket;
    input_stream read_buf;
    output_stream write_buf;

    serialization::byte_buffer data_buffer;
    int remaining_bytes {0};
    size_t read_so_far_bytes {0};
};

}


#endif // CONNECTION_H
