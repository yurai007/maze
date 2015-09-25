#include "game_server.hpp"
#include "../common/message_dispatcher.hpp"
#include "../common/byte_buffer.hpp"

namespace networking
{

void game_server::init(std::shared_ptr<core::server_maze> maze,
					   std::shared_ptr<core::server_world_manager> manager)
{
	auto dispatcher = std::make_shared<networking::message_dispatcher>();

	dispatcher->add_handler(
				[&](messages::get_chunk &msg)
	{
		logger_.log("game_server: recieved get_chunk for [%d,%d] [%d,%d]", msg.ld_x, msg.ld_y,
					msg.ru_x, msg.ru_y);
		messages::get_chunk_response response(maze->get_chunk(msg.ld_x, msg.ld_y,
															  msg.ru_x, msg.ru_y));
		sender.send(response);
		logger_.log("game_server: send get_chunk_response");
	});

	dispatcher->add_handler(
				[&](messages::position_changed &msg)
	{
		logger_.log("game_server: recieved position_changed for player id = %d, [%d,%d] --> [%d,%d]",
					msg.player_id, msg.old_x, msg.old_y, msg.new_x, msg.new_y);

		manager->update_player_position(msg.player_id, msg.old_x, msg.old_y, msg.new_x, msg.new_y);
		messages::position_changed_response response;

		sender.send(response);
		logger_.log("game_server: send position_changed_response");
	});

	dispatcher->add_handler(
				[&](messages::get_enemies_data &msg)
	{
		logger_.log("game_server: recieved get_enemies_data");

		messages::get_enemies_data_response response(manager->get_enemies_data(true));

		logger_.log("game_server: get_enemies_data before sending. Content dump:");
		size_t i = 0;
		for (; i < response.content.size(); i += 3)
		{
			if (i != 0 && (i % 30 == 0))
				logger_.log_in_place("{%d, %d, %d}\n", response.content[i], response.content[i+1],
					response.content[i+2]);
			else
				logger_.log_in_place("{%d, %d, %d} ", response.content[i], response.content[i+1],
					response.content[i+2]);
		}
		if ((i-3)%30 != 0)
			logger_.log_in_place("\n");
		sender.send(response);
		logger_.log("game_server: send get_enemies_data_response");
	});

	dispatcher->add_handler(
				[&](messages::get_players_data &msg)
	{
		logger_.log("game_server: recieved get_players_data");
		messages::get_players_data_response response = manager->allocate_player_for_client();
		sender.send(response);
		logger_.log("game_server: send get_players_data_response for player id = %d, pos = {%d, %d}",
					response.id, response.posx, response.posy);
	});

	main_server.add_dispatcher(dispatcher);
}

void game_server::run()
{
	main_server.run();
}

void game_server::stop()
{
	main_server.stop();
}

}

