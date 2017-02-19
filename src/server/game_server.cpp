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

        if (!((msg.new_x - msg.old_x == 0 ) || (msg.new_y - msg.old_y == 0)))
        {
            logger_.log("game_server: bad position_changed");
            throw std::runtime_error("bad position_changed");
        }

        messages::position_changed_response response;
        if (manager->update_player_position_if_possible(msg.player_id, msg.old_x, msg.old_y,
                                                        msg.new_x, msg.new_y))
        {
            logger_.log("game_server: position_changed OK");
        }
        else
        {
            response.content = "NOK";
            logger_.log("game_server: position_changed failed");
        }
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

    dispatcher->add_handler(
                [&](messages::client_shutdown &msg)
    {
        logger_.log("game_server: recieved client_shutdown from player_id = %d", msg.player_id);
        messages::client_shutdown_response response;
        manager->shutdown_player(msg.player_id);
        return response;
    });

    dispatcher->add_handler(
                [&](messages::fireball_triggered &msg)
    {
        logger_.log("game_server: recieved fireball_triggered from player_id = %d", msg.player_id);

        if (!(msg.direction == 'L' || msg.direction == 'R' || msg.direction == 'U' || msg.direction == 'D'))
        {
            logger_.log("game_server: bad fireball_triggered");
            throw std::runtime_error("bad fireball_triggered");
        }

        messages::fireball_triggered_response response;
        if (manager->allocate_new_fireball_if_possible(msg.player_id, msg.pos_x, msg.pos_y, msg.direction))
        {
            logger_.log("game_server: fireball triggering OK");
        }
        else
        {
            response.content = "NOK";
            logger_.log("game_server: fireball triggering failed");
        }
        return response;
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

asio_reactor &game_server::get_reactor()
{
    return main_server.get_reactor();
}

}

