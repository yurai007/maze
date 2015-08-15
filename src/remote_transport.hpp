#ifndef REMOTE_TRANSPORT_HPP
#define REMOTE_TRANSPORT_HPP

#include <memory>
#include "byte_buffer.hpp"
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
        msg.serialize_to_buffer(data);
        m_server.send_on_current_connection(data);
    }
private:
    server &m_server;
};

}

}


#endif // REMOTE_TRANSPORT_HPP
