#ifndef GAME_SERVER_HPP
#define GAME_SERVER_HPP

#include <memory>
#include "maze.hpp"
#include "byte_buffer.hpp"
#include "server.h"
#include "remote_transport.hpp"

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
	get_chunk_response() = default;

	get_chunk_response(const std::string &chunk)
	{
		content = chunk;
	}

	void serialize_to_buffer(serialization::byte_buffer &buffer) const
	{
		buffer.put_string(content);
	}

	void deserialize_from_buffer(serialization::byte_buffer &buffer)
	{
		content = buffer.get_string();
	}

	static int message_id()
	{
		return 1;
	}

	std::string content;
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
	position_changed_response() = default;

	void serialize_to_buffer(serialization::byte_buffer &buffer) const
	{
		buffer.put_string(content);
	}

	void deserialize_from_buffer(serialization::byte_buffer &buffer)
	{
		content = buffer.get_string();
	}

	static int message_id()
	{
		return 3;
	}

	std::string content {"OK"};
};

}

class game_server
{
public:
    game_server();
    void init(std::shared_ptr<core::maze> maze);
    void run();
    void stop();

private:
    server main_server {5555};
    remote_transport::sender sender {main_server};
};

}

#endif // GAME_SERVER_HPP
