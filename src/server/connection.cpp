#include <iostream>
#include "connection.hpp"
#include "server.hpp"

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
                       [this](const auto &error, size_t bytes){ this->handle_read(error, bytes); });
}

void connection::stop()
{
    socket.close();
}

void connection::send(const serialization::byte_buffer &data)
{
    data_buffer = data;
    async_write(socket,
                buffer(&data_buffer.m_byte_buffer[0], data_buffer.offset),
                [this](const auto &error, size_t bytes){ this->handle_write(error, bytes); });
}

// TCP doesn't ensure that 1 x send N bytes == 1 x recv N bytes
void connection::handle_read(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (!error)
    {
        m_server.current_connection = id;
        unsigned short msg_length = 0;

        if (remaining_bytes == 0)
        {
            memcpy(&msg_length, &data_buffer.m_byte_buffer[0], sizeof(msg_length));

            remaining_bytes =  msg_length + sizeof_msg_size - bytes_transferred;
            read_so_far_bytes = bytes_transferred;
        }
        else
        {
            remaining_bytes -= bytes_transferred;
            read_so_far_bytes += bytes_transferred;
        }

        if (remaining_bytes > 0)
        {
            logger_.log("connection with id = %d: recieved %d B and expected %d B. "
                        "Waiting for next %d B",
                        socket.native_handle(), bytes_transferred,
                        msg_length + sizeof_msg_size, remaining_bytes);

            socket.async_read_some(buffer(&data_buffer.m_byte_buffer[read_so_far_bytes], remaining_bytes),
                                   [this](const auto &error_, size_t bytes){
                                                                this->handle_read(error_, bytes); });
        }
        else
        {
            logger_.log("connection with id = %d: recieved %d B and expected %d B. Got full msg",
                        socket.native_handle(), bytes_transferred, bytes_transferred);

            data_buffer.offset = sizeof_msg_size;
            m_server.dispatch_msg_from_buffer(data_buffer);
        }
    }
    else
    {
        m_server.remove_connection(id);
    }
}

void connection::handle_write(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (!error)
    {
        logger_.log("connection with id = %d: sent %d B", socket.native_handle(),
                                     bytes_transferred);
        socket.async_read_some(buffer(data_buffer.m_byte_buffer, serialization::max_size),
                                 [this](const auto &error_, size_t bytes){
                                                                this->handle_read(error_, bytes); });
    }
    else
    {
        m_server.remove_connection(id);
    }
}

}

