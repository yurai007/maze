#ifndef CONNECTION_H
#define CONNECTION_H

#include <iostream>
#include "server.hpp"

#include "../common/logger.hpp"
#include "../common/byte_buffer.hpp"

namespace networking
{

constexpr static int msg_size = sizeof(unsigned short);

template<class Reactor>
class server;

template<class Reactor>
class connection
{
public:
    connection(const connection&) = delete;
    connection& operator=(const connection&) = delete;


    connection(typename Reactor::connected_socket socket_, server<Reactor> &server_)
        : m_server(server_),
          socket(std::move(socket_))
    {
    }

    auto &get_socket()
    {
        return socket;
    }

    void start()
    {
        read_buf.read_at_least_one_byte(socket, &data_buffer.m_byte_buffer[0], serialization::max_size,
                          [this](const auto &error, auto bytes){ this->process(error, bytes);
                     });
    }

    void stop()
    {
        socket.close();
    }

    unsigned id {0};

private:

    void process(const typename Reactor::connected_socket::error_type& error, size_t bytes_transferred)
    {
        if (!error)
        {
            unsigned short msg_length = 0;
            if (remaining_bytes == 0)
            {
                memcpy(&msg_length, &data_buffer.m_byte_buffer[0], sizeof msg_length);
                remaining_bytes =  msg_length + msg_size - bytes_transferred;
                read_so_far_bytes = bytes_transferred;
            }
            else
            {
                remaining_bytes -= bytes_transferred;
                read_so_far_bytes += bytes_transferred;
            }

            if (remaining_bytes > 0)
            {
                if (server<Reactor>::debug)
                {
                    logger_.log_debug("connection with id = %d: recieved %d B and expected %d B. "
                                      "Waiting for next %d B",
                                      socket.native_handle(), bytes_transferred,
                                      msg_length + msg_size, remaining_bytes);
                }
                read_buf.read_at_least_one_byte(socket,
                              &data_buffer.m_byte_buffer[read_so_far_bytes],
                              remaining_bytes,
                              [this](const auto &error_, auto bytes){
                                      this->process(error_, bytes);
                         });
            }
            else
            {
                if (server<Reactor>::debug)
                {
                    logger_.log_debug("connection with id = %d: recieved %d B and expected %d B. Got full msg",
                                      socket.native_handle(), bytes_transferred, bytes_transferred);
                }
                data_buffer.offset = msg_size;

                // TO DO: connections as Boost.Intrusive list??
                auto data_out = m_server.get_dispatcher()->dispatch_req_get_resp(data_buffer);
                data_buffer = data_out;

                write_buf.write_all(socket, &data_buffer.m_byte_buffer[0], data_buffer.offset,
                                [this](const auto &error_, auto bytes_transferred_)
                                {
                                       if (!error_)
                                       {
                                           if (server<Reactor>::debug)
                                           {
                                               logger_.log_debug("connection with id = %d: sent %d B",
                                                                 socket.native_handle(),
                                                                 bytes_transferred_);
                                           }
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

    server<Reactor> &m_server;
    typename Reactor::connected_socket socket;
    typename Reactor::input_stream read_buf;
    typename Reactor::output_stream write_buf;

    serialization::byte_buffer data_buffer;
    int remaining_bytes {0};
    size_t read_so_far_bytes {0};
};

}


#endif // CONNECTION_H
