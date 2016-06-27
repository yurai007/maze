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

class connection
{
public:

    connection(const connection&) = delete;
    connection& operator=(const connection&) = delete;

    connection(io_service& io_service, server &server_);
    tcp::socket& get_socket();
    void start();
    void stop();
    void send(const serialization::byte_buffer &data);

    unsigned id {0};

private:

    void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
    void handle_write(const boost::system::error_code& error, size_t bytes_transferred);

    server &m_server;
    tcp::socket socket;
    serialization::byte_buffer data_buffer;
    int remaining_bytes {0};
    int read_so_far_bytes {0};
};

}


#endif // CONNECTION_H
