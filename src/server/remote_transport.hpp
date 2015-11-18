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
        data.put_unsigned_short(0);
        data.put_char(msg.message_id());
        msg.serialize_to_buffer(data);

        assert(data.get_size() >= 2);
        assert(data.get_size() - 2 < 256*256);
        unsigned short size = (unsigned short)(data.get_size() - 2);
        memcpy(&data.m_byte_buffer[0], &size, sizeof(size));

        m_server.send_on_current_connection(data);
    }
private:
    server &m_server;
};

}

}

#endif // REMOTE_TRANSPORT_HPP
