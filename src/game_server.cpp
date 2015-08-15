#include "game_server.hpp"
#include "message_dispatcher.hpp"
#include "byte_buffer.hpp"

namespace networking
{

// instead saturating 1Gb I should trend to low latency and high req/s
game_server::game_server(std::shared_ptr<core::maze> maze)
{
	auto dispatcher = std::make_shared<networking::message_dispatcher>();

	dispatcher->add_handler(
				[&](messages::get_chunk const& msg)
	{
		messages::get_chunk_response response(maze->get_chunk(msg.ld_x, msg.ld_y,
															  msg.ru_x, msg.ru_y));
		sender.send(response);
		logger_.log("game_server: send get_chunk_response for [%d,%d] [%d,%d]", msg.ld_x, msg.ld_y,
					msg.ru_x, msg.ru_y);
	});

	dispatcher->add_handler(
				[&](messages::position_changed const& msg)
	{
		maze->move_field(msg.old_x, msg.old_y, msg.new_x, msg.new_y);
		messages::position_changed_response response;

		sender.send(response);
		logger_.log("game_server: send position_changed_response for [%d,%d] --> [%d,%d]",
					msg.old_x, msg.old_y, msg.new_x, msg.new_y);
	});

	main_server.add_dispatcher(dispatcher);
}

}

