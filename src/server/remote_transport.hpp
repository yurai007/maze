#ifndef REMOTE_TRANSPORT_HPP
#define REMOTE_TRANSPORT_HPP

#include <memory>
#include "../common/byte_buffer.hpp"
#include "server.h"

#include <boost/date_time/posix_time/posix_time.hpp>

namespace networking
{

namespace remote_transport
{

class sender
{
public:
    sender(server &main_server)
        : m_server(main_server) {}

    template<class Msg>
    void send(Msg &msg)
    {
        serialization::byte_buffer data;
        data.put_char(0);
        data.put_char(msg.message_id());
        msg.serialize_to_buffer(data);
        unsigned char size = (unsigned char)(data.get_size() - 1);
        data.m_byte_buffer[0] = size;
        m_server.send_on_current_connection(data);
    }
private:
    server &m_server;
};

}

}

#endif // REMOTE_TRANSPORT_HPP
