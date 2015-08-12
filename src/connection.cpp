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
    socket.async_read_some(buffer(data_buffer.m_byte_buffer, serialization::max_size),
                             boost::bind(&connection::handle_read, this, placeholders::error,
                                         placeholders::bytes_transferred));
}

// TCP doesn't ensure that 1 x send N bytes == 1 x recv N bytes
void connection::handle_read(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (!error)
    {
        logger_.log("connection %d: recieved %d B", socket.native_handle(),
                                     bytes_transferred);

        if (remaining_bytes == 0)
        {
            unsigned char msg_length = data_buffer.m_byte_buffer[0];
            remaining_bytes =  msg_length + 1 - bytes_transferred;
            current = bytes_transferred;
            logger_.log("connection %d: expected %d B",
                                         socket.native_handle(), msg_length);
        }
        else
        {
            remaining_bytes -= bytes_transferred;
            current += bytes_transferred;
        }

        if (remaining_bytes > 0)
        {
            logger_.log("connection %d: waiting for next %d B",
                                         socket.native_handle(), remaining_bytes);
            socket.async_read_some(buffer(&data_buffer.m_byte_buffer[current], remaining_bytes),
                                   boost::bind(&connection::handle_read, this, placeholders::error,
                                               placeholders::bytes_transferred));
        }
        else
        {
            logger_.log("connection %d: recieved full msg", socket.native_handle());
            // 1. id = 'Recieved msg with type' from data_buffer.
            //    For each msg_type in registered_type_from_handlers
            //    if( id == msg_type::id())
            //       msg = deserialize and go to 2.
            //
            // 2. sender.current_socket = socket;
            //    dispatcher.dispatch(msg);
            //

//            async_write(socket,
//                        buffer(data_buffer.m_byte_buffer, current),
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
        logger_.log("connection %d: sent %d B", socket.native_handle(),
                                     bytes_transferred);
        socket.async_read_some(buffer(data_buffer.m_byte_buffer, serialization::max_size),
                                 boost::bind(&connection::handle_read, this, placeholders::error,
                                             placeholders::bytes_transferred));
    }
    else
    {
        m_server.remove_connection(shared_from_this());
    }
}

}
