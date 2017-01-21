#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

#include "../common/messages.hpp"
#include "async_logger.hpp"

using boost::asio::ip::tcp;

namespace networking
{
/*
 * This is synchronous client. The intention is to have only one application thread
   with all stuff.

 * Funny. Methods differs only in return type cannot by overloaded:)
*/
class client
{
public:
    client(const std::string &ip_address);

    void send_request(messages::get_chunk &msg)
    {
         send(msg);
    }

    void send_request(messages::position_changed &msg)
    {
        send(msg);
    }

    void send_request(messages::get_resources_data &msg)
    {
        send(msg);
    }

    void send_request(messages::get_id &msg)
    {
         send(msg);
    }

    void send_request(messages::client_shutdown &msg)
    {
         send(msg);
    }

    messages::get_chunk_response read_get_chunk_response()
    {
        return read<messages::get_chunk_response>();
    }

    messages::position_changed_response read_position_changed_response()
    {
        return read<messages::position_changed_response>();
    }

    messages::get_resources_data_response read_get_resources_data_response()
    {
        return read<messages::get_resources_data_response>();
    }

    messages::get_id_response read_get_id_response()
    {
        return read<messages::get_id_response>();
    }

private:

    template<class Msg>
    void send(Msg &msg)
    {
        serialization::byte_buffer serialized_msg;
        serialized_msg.put_unsigned_short(0);
        serialized_msg.put_char(msg.message_id());
        msg.serialize_to_buffer(serialized_msg);

        assert(serialized_msg.get_size() >= 2);
        assert(serialized_msg.get_size() - 2 < 256*256);
        unsigned short size = (unsigned short)(serialized_msg.get_size() - 2);
        memcpy(&serialized_msg.m_byte_buffer[0], &size, sizeof(size));

        boost::system::error_code error;
        size_t send_bytes = boost::asio::write(socket, boost::asio::buffer(serialized_msg.m_byte_buffer,
                                                             serialized_msg.offset), error);
        if (error)
        {
            logger_.log("client: write error = %d", error.value());
            assert(false);
        }
        assert(send_bytes == static_cast<size_t>(serialized_msg.offset));
    }

    template<class Msg>
    Msg read()
    {
        boost::system::error_code error;
        serialization::byte_buffer deserialized_msg;
        size_t recieved_bytes = socket.read_some(boost::asio::buffer(
                                                       deserialized_msg.m_byte_buffer), error);
        if (error)
        {
            logger_.log("client: read error = %d", error.value());
            assert(false);
        }
        assert(recieved_bytes > 0);

        Msg msg;
        deserialized_msg.get_unsigned_short();
        char msg_type = deserialized_msg.get_char();
        if (Msg::message_id() != msg_type)
            logger_.log("client: recieved message with wrong type. Expected message_id = %d", msg_type);

        msg.deserialize_from_buffer(deserialized_msg);
        return msg;
    }

    void stop();

    boost::asio::io_service io_service;
    tcp::resolver resolver {io_service};
    tcp::socket socket {io_service};
    const std::string port {"5555"};
};

}

#endif // CLIENT_HPP
