#include "client.hpp"

namespace networking
{

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
}

}
