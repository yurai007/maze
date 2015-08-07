#include "game_server.hpp"
#include "message_dispatcher.hpp"
#include "remote_transport.hpp"

namespace networking
{

namespace messages
{

struct get_chunk
{
};

struct get_chunk_response
{
};

struct position_changed
{
};

struct position_changed_response
{
};

}


// instead saturating 1Gb I should trend to low latency and high req/s
game_server::game_server(std::shared_ptr<core::maze> maze)
{
//	remote_transport::sender sender;
//	networking::message_dispatcher dispatcher;

//	dispatcher.add_handler(
//				[&](messages::get_chunk const& msg)
//	{
//		messages::get_chunk_response response(maze->get_chunk(msg.ld_x, msg.ld_y,
//															  msg.ru_x, msg.ru_y));
//		sender.send(response);
//	});

//	dispatcher.add_handler(
//				[&](messages::position_changed const& msg)
//	{
//		maze->move_field(msg.old_x, msg.old_y, msg.new_x, msg.new_y);
//		messages::position_changed_response response(msg);

//		sender.send(response);
//	});

//	server.add_dispatcher(dispatcher);
}

}

