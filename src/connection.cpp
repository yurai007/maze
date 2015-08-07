#include <iostream>
#include <boost/bind.hpp>

#include "connection.h"
#include "server.h"

namespace networking
{

connection::connection(io_service& io_service, server &server_)
    : m_server(server_),
      socket(io_service)
{
}

tcp::socket& connection::get_socket()
{
    return socket;
}

void connection::start()
{
    socket.async_read_some(buffer(data_buffer, max_buffer_size),
                             boost::bind(&connection::handle_read, this, placeholders::error,
                                         placeholders::bytes_transferred));
}

// TCP doesn't ensure that 1 x send N bytes == 1 x recv N bytes
void connection::handle_read(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (!error)
    {
        framework::logger::get().log("connection %d: recieved %d B", socket.native_handle(),
                                     bytes_transferred);

        if (remaining_bytes == 0)
        {
            unsigned char msg_length = data_buffer[0];
            remaining_bytes =  msg_length + 1 - bytes_transferred;
            current = bytes_transferred;
            framework::logger::get().log("connection %d: expected %d B",
                                         socket.native_handle(), msg_length);
        }
        else
        {
            remaining_bytes -= bytes_transferred;
            current += bytes_transferred;
        }

        if (remaining_bytes > 0)
        {
            framework::logger::get().log("connection %d: waiting for next %d B",
                                         socket.native_handle(), remaining_bytes);
            socket.async_read_some(buffer(&data_buffer[current], remaining_bytes),
                                   boost::bind(&connection::handle_read, this, placeholders::error,
                                               placeholders::bytes_transferred));
        }
        else
        {
            framework::logger::get().log("connection %d: recieved full msg", socket.native_handle());

            // do sth with msg
            //dispatcher.

//            async_write(socket,
//                        buffer(data_buffer, current),
//                        boost::bind(&connection::handle_write, this, placeholders::error,
//                                    placeholders::bytes_transferred));
        }
    }
    else
    {
        m_server.remove_connection(shared_from_this());
    }
}

void connection::handle_write(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (!error)
    {
        framework::logger::get().log("connection %d: sent %d B", socket.native_handle(),
                                     bytes_transferred);
        socket.async_read_some(buffer(data_buffer, max_buffer_size),
                                 boost::bind(&connection::handle_read, this, placeholders::error,
                                             placeholders::bytes_transferred));
    }
    else
    {
        m_server.remove_connection(shared_from_this());
    }
}

}

