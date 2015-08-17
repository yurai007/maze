#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <vector>

using boost::asio::ip::tcp;

boost::asio::io_service io_service;
tcp::resolver resolver(io_service);
tcp::socket m_socket(io_service);


void connect_handler(const boost::system::error_code &p_error_code)
{
    if (!p_error_code)
    {
        // messages::get_chunk with payload [1,1] [1,1]
        std::vector<unsigned char> data_buffer_request = {1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0};
        boost::system::error_code error;
        size_t len = m_socket.write_some(boost::asio::buffer(data_buffer_request), error);
        std::cout << "Send some data\n";
//        std::vector<unsigned char> data_buffer_response(128);
//        len = m_socket.read_some(boost::asio::buffer(data_buffer_response), error);
//        for (size_t i =0;i<data_buffer_response.size(); i++)
//            std::cout << data_buffer_response[i];
    }
}

void resolve_handler(const boost::system::error_code &error_code,
                     tcp::resolver::iterator endpoint_iterator)
{
    if (!error_code)
    {
        m_socket.async_connect(*endpoint_iterator, connect_handler);
    }
    else
    {
        std::cout << "Resolve handler: Some error occured: " << error_code.value() << "\n";
        exit(1);
    }
}

int main(int argc, char* argv[])
{
    try
    {
        tcp::resolver::query query(tcp::tcp::v4(), "127.0.0.1", "5555");

        resolver.async_resolve(query, boost::bind( &resolve_handler,
             boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred) );

        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cout << "Problem!\n";
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
