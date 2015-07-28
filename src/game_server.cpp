#include "game_server.hpp"
#include "message_dispatcher.hpp"
#include "remote_transport.hpp"

namespace networking
{

// instead saturating 1Gb I should trend to low latency and high req/s
game_server::game_server()
{
	remote_transport::sender sender;
	remote_transport::receiver receiver;
	dispatching::dispatcher dispatcher(std::make_shared<remote_transport::receiver>(receiver));

	while (true)
	{
		dispatcher
			.handle<messages::get_chunk>(
				[&](messages::get_chunk const& msg)
		{
			messages::get_chunk_response response = maze->get_chunk(msg.ld_x, msg.ld_y,
																	msg.ru_x, msg.ru_y);
			sender.send(response);
		})
		.handle<messages::position_changed>(
				[&](messages::position_changed const& msg)
		{
			maze->update_field(msg.old_x, msg.old_y, msg.new_x, msg.new_y);
			messages::position_changed_response response(msg);

			sender.send(response);
		});
	}
}

}
