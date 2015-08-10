#include "game_server.hpp"
#include "message_dispatcher.hpp"
#include "remote_transport.hpp"
#include "byte_buffer.hpp"

namespace networking
{

namespace messages
{

struct get_chunk
{
	int ld_x, ld_y, ru_x, ru_y;

	void serialize_to_buffer(serialization::byte_buffer &buffer) const
	{
		buffer.put_int(ld_x);
		buffer.put_int(ld_y);
		buffer.put_int(ru_x);
		buffer.put_int(ru_y);
	}

	void deserialize_from_buffer(serialization::byte_buffer &buffer)
	{
		ld_x = buffer.get_int();
		ld_y = buffer.get_int();
		ru_x = buffer.get_int();
		ru_y = buffer.get_int();
	}

	static int message_id()
	{
		return 0;
	}
};

struct get_chunk_response
{
	get_chunk_response(const std::vector<std::string> &chunk)
	{
	}

	static int message_id()
	{
		return 1;
	}
};

struct position_changed
{
	int old_x, old_y, new_x, new_y;

	void serialize_to_buffer(serialization::byte_buffer &buffer) const
	{
		buffer.put_int(old_x);
		buffer.put_int(old_y);
		buffer.put_int(new_x);
		buffer.put_int(new_y);
	}

	void deserialize_from_buffer(serialization::byte_buffer &buffer)
	{
		old_x = buffer.get_int();
		old_y = buffer.get_int();
		new_x = buffer.get_int();
		new_y = buffer.get_int();
	}

	static int message_id()
	{
		return 2;
	}
};

struct position_changed_response
{
	position_changed_response(position_changed const& msg)
	{
	}

	static int message_id()
	{
		return 3;
	}
};

}

// instead saturating 1Gb I should trend to low latency and high req/s
game_server::game_server(std::shared_ptr<core::maze> maze)
{
	remote_transport::sender sender;
	networking::message_dispatcher dispatcher;

	dispatcher.add_handler(
				[&](messages::get_chunk const& msg)
	{
		messages::get_chunk_response response(maze->get_chunk(msg.ld_x, msg.ld_y,
															  msg.ru_x, msg.ru_y));
		sender.send(response);
	});

	dispatcher.add_handler(
				[&](messages::position_changed const& msg)
	{
		maze->move_field(msg.old_x, msg.old_y, msg.new_x, msg.new_y);
		messages::position_changed_response response(msg);

		sender.send(response);
	});

//	server.add_dispatcher(dispatcher);
}

}

