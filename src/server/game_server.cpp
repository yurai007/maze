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
    auto dispatcher = smart::smart_make_shared<networking::message_dispatcher>();

	dispatcher->add_handler(
				[&](messages::get_chunk &msg)
	{
        manager->repair_if_uncorrect_enemies();
		logger_.log("game_server: recieved get_chunk for [%u,%u] [%u,%u]", msg.ld_x, msg.ld_y,
					msg.ru_x, msg.ru_y);
		messages::get_chunk_response response(maze->get_chunk(msg.ld_x, msg.ld_y,
															  msg.ru_x, msg.ru_y));
        return response;
	});

	dispatcher->add_handler(
				[&](messages::position_changed &msg)
	{
		logger_.log("game_server: recieved position_changed for player id = %d, [%d,%d] --> [%d,%d]",
					msg.player_id, msg.old_x, msg.old_y, msg.new_x, msg.new_y);

		manager->update_player_position(msg.player_id, msg.old_x, msg.old_y, msg.new_x, msg.new_y);
		messages::position_changed_response response;
        return response;
	});

	dispatcher->add_handler(
				[&](messages::get_players_data &)
	{
		logger_.log("game_server: recieved get_players_data");

		manager->repair_if_uncorrect_players();
		messages::get_players_data_response response(manager->get_players_data());
        return response;
	});

    dispatcher->add_handler(
                [&](messages::get_resources_data &)
    {
        logger_.log("game_server: recieved get_resources_data");

        messages::get_resources_data_response response(manager->get_resources_data());
        return response;
    });

	dispatcher->add_handler(
				[&](messages::get_id &)
	{
		logger_.log("game_server: recieved get_id");
		messages::get_id_response response;
		response.player_id = manager->allocate_data_for_new_player();

		logger_.log("player_id = %d", response.player_id);
        return response;
	});

//	dispatcher->add_handler(
//				[&](messages::client_shutdown &msg)
//	{
//		logger_.log("game_server: recieved client_shutdown from player_id = %d", msg.player_id);
//		manager->shutdown_player(msg.player_id);
//        // TO DO: remove this - dummy network overhead involved
//        int dummy_response {-1};
//        return dummy_response;
//	});

//    dispatcher->add_handler(
//                [&](messages::fireball_triggered &msg)
//    {
//        logger_.log("game_server: recieved fireball_triggered from player_id = %d", msg.player_id);
//        manager->allocate_data_for_new_fireball(msg.player_id, msg.pos_x, msg.pos_y, msg.direction);
//        // TO DO: remove this - dummy network overhead involved
//        int dummy_response {-1};
//        return dummy_response;
//    });

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

