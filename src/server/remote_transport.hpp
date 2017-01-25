#ifndef REMOTE_TRANSPORT_HPP
#define REMOTE_TRANSPORT_HPP

#include "../common/byte_buffer.hpp"
#include "server.hpp"

namespace networking
{

namespace remote_transport
{

//class sender
//{
//public:
//    sender(server &main_server)
//        : m_server(main_server) {}

//    template<class Msg>
//    void send(Msg &msg)
//    {
//        serialization::byte_buffer data;
//        data.put_unsigned_short(0);
//        data.put_char(msg.message_id());
//        msg.serialize_to_buffer(data);

//        assert(data.get_size() >= sizeof_msg_size);
//        assert(data.get_size() - sizeof_msg_size < 256*256);
//        unsigned short size = (unsigned short)(data.get_size() - sizeof_msg_size);
//        memcpy(&data.m_byte_buffer[0], &size, sizeof(size));

//        m_server.send_on_current_connection(data);
//    }
//private:
//    server &m_server;
//};

}

}

#endif // REMOTE_TRANSPORT_HPP
