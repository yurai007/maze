#ifndef REMOTE_TRANSPORT_HPP
#define REMOTE_TRANSPORT_HPP

namespace networking
{

namespace remote_transport
{

class sender
{
public:
    template<class Msg>
    void send(Msg &msg)
    {

    }
};

class receiver
{
public:
    void wait_on_msg();
    bool received_msg_with_type(char id);

    template<class Msg>
    void deserialize_from_buffer(Msg &msg)
    {

    }
};

}

}


#endif // REMOTE_TRANSPORT_HPP
