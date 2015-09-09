#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

#include "../common/messages.hpp"
#include "../common/logger.hpp"

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
    client();

    void send_request(messages::get_chunk &msg)
    {
         send(msg);
    }

    void send_request(messages::position_changed &msg)
    {
        send(msg);
    }

    void send_request(messages::get_enemies_data &msg)
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

    messages::get_enemies_data_response read_get_enemies_data_response()
    {
        return read<messages::get_enemies_data_response>();
    }

private:

    template<class Msg>
    void send(Msg &msg)
    {
        serialization::byte_buffer serialized_msg;
        serialized_msg.put_char(0);
        serialized_msg.put_char(msg.message_id());
        msg.serialize_to_buffer(serialized_msg);
        unsigned char size = (unsigned char)(serialized_msg.get_size() - 1);
        serialized_msg.m_byte_buffer[0] = size;

        boost::system::error_code error;

        size_t send_bytes = socket.write_some(boost::asio::buffer(serialized_msg.m_byte_buffer,
                                                             serialized_msg.offset), error);
        assert(!error);
        logger_.log("Send message with %d B size to server", send_bytes);
    }

    template<class Msg>
    Msg read()
    {
        boost::system::error_code error;
        serialization::byte_buffer deserialized_msg;
        size_t recieved_bytes = socket.read_some(boost::asio::buffer(
                                                       deserialized_msg.m_byte_buffer), error);
        assert(!error);
        logger_.log("Recieved message with %d B size from server", recieved_bytes);

        Msg msg;
        deserialized_msg.get_char(); //char msg_size =
        char msg_type = deserialized_msg.get_char();
        if (Msg::message_id() != (int)msg_type)
            logger_.log("Recieved message with wrong type. Expected message_id = %d", msg_type);

        msg.deserialize_from_buffer(deserialized_msg);
        return msg;
    }

    boost::asio::io_service io_service;
    tcp::resolver resolver {io_service};
    tcp::socket socket {io_service};
};

}

#endif // CLIENT_HPP
