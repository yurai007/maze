#include "game_server.hpp"
#include "../common/messages.hpp"
#include "../common/message_dispatcher.hpp"
#include "../common/byte_buffer.hpp"

namespace networking
{

void game_server::init(smart::fit_smart_ptr<core::server_maze> maze,
                       smart::fit_smart_ptr<core::server_world_manager> manager)
{
    assert(maze != nullptr && manager != nullptr);
	auto dispatcher = std::make_shared<networking::message_dispatcher>();

	dispatcher->add_handler(
				[&](messages::get_chunk &msg)
	{
		logger_.log("game_server: recieved get_chunk for [%u,%u] [%u,%u]", msg.ld_x, msg.ld_y,
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
		logger_.log("game_server: recieved get_enemies_data from player = %d", msg.player_id);

		manager->repair_if_uncorrect_enemies();
		messages::get_enemies_data_response response(manager->get_enemies_data());

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
				[&](messages::get_players_data &)
	{
		logger_.log("game_server: recieved get_players_data");

		manager->repair_if_uncorrect_players();
		messages::get_players_data_response response(manager->get_players_data());

//		logger_.log("game_server: get_players_data before sending. Content dump:");
//		size_t i = 0;
//		for (; i < response.content.size(); i += 3)
//		{
//			if (i != 0 && (i % 30 == 0))
//				logger_.log_in_place("{%d, %d, %d}\n", response.content[i], response.content[i+1],
//					response.content[i+2]);
//			else
//				logger_.log_in_place("{%d, %d, %d} ", response.content[i], response.content[i+1],
//					response.content[i+2]);
//		}
//		if ((i-3)%30 != 0)
//			logger_.log_in_place("\n");

		sender.send(response);
		logger_.log("game_server: send get_players_data_response");
	});

	dispatcher->add_handler(
				[&](messages::get_id &)
	{
		logger_.log("game_server: recieved get_id");
		messages::get_id_response response;
		response.player_id = manager->allocate_data_for_new_player();

		logger_.log("player_id = %d", response.player_id);
		sender.send(response);
		logger_.log("game_server: send get_id_response");
	});

	dispatcher->add_handler(
				[&](messages::client_shutdown &msg)
	{
		logger_.log("game_server: recieved client_shutdown from player_id = %d", msg.player_id);
		manager->shutdown_player(msg.player_id);
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

io_service &game_server::get_io_service()
{
	return main_server.get_io_service();
}

}

