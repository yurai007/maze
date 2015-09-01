#include "client.hpp"

namespace networking
{

// that's temporary constructor implementation to test client
client::client()
{
    tcp::resolver::query query(tcp::tcp::v4(), "127.0.0.1", "5555");
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::resolver::iterator end;
    boost::system::error_code error = boost::asio::error::host_not_found;

    while (error && endpoint_iterator != end)
    {
        socket.close();
        socket.connect(*endpoint_iterator++, error);
    }
    assert(!error);

//    if (error)
//        throw boost::system::system_error(error);

//    for (;;)
//    {
//        boost::array<char, 128> buf;
//        boost::system::error_code error;

//        size_t len = socket.read_some(boost::asio::buffer(buf), error);

//        if (error == boost::asio::error::eof)
//            break; // Connection closed cleanly by peer.
//        else if (error)
//            throw boost::system::system_error(error); // Some other error.

//        std::cout.write(buf.data(), len);
//    }
}

//void client::send_request(messages::get_chunk &msg)
//{
//    send(msg);
//}



}
