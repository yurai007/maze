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
    read_buf.read_at_least_one_byte(socket, &data_buffer.m_byte_buffer[0], serialization::max_size,
                      [this](const auto &error, auto bytes){ this->process(error, bytes);
                 });
}

void connection::stop()
{
    socket.close();
}

// TCP doesn't ensure that 1 x send N bytes == 1 x recv N bytes
void connection::process(const error_type& error, size_t bytes_transferred)
{
    if (!error)
    {
        unsigned short msg_length = 0;
        if (remaining_bytes == 0)
        {
            memcpy(&msg_length, &data_buffer.m_byte_buffer[0], sizeof msg_length);
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
//            logger_.log_debug("connection with id = %d: recieved %d B and expected %d B. "
//                        "Waiting for next %d B",
//                        socket.native_handle(), bytes_transferred,
//                        msg_length + sizeof_msg_size, remaining_bytes);

            read_buf.read_at_least_one_byte(socket,
                          &data_buffer.m_byte_buffer[read_so_far_bytes],
                          remaining_bytes,
                          [this](const auto &error_, auto bytes){
                                  this->process(error_, bytes);
                     });
        }
        else
        {
//            logger_.log_debug("connection with id = %d: recieved %d B and expected %d B. Got full msg",
//                        socket.native_handle(), bytes_transferred, bytes_transferred);

            data_buffer.offset = sizeof_msg_size;
            auto data_out = m_server.m_dispatcher->dispatch_req_get_resp(data_buffer);
            data_buffer = data_out;

            write_buf.write_all(socket, &data_buffer.m_byte_buffer[0], data_buffer.offset,
                            [this](const auto &error_, auto bytes_transferred_)
                            {
                                   if (!error_)
                                   {
//                                       logger_.log_debug("connection with id = %d: sent %d B",
//                                                         socket.native_handle(),
//                                                         bytes_transferred_);

                                       read_buf.read_at_least_one_byte(socket,
                                                    &data_buffer.m_byte_buffer[0],
                                                    serialization::max_size,
                                                    [this](const auto &_error, auto bytes){
                                                            this->process(_error, bytes);
                                               });
                                   }
                                   else
                                       m_server.remove_connection(id);
                        });
        }
    }
    else
        m_server.remove_connection(id);
}

}

