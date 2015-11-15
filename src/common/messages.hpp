#ifndef MESSAGES_HPP
#define MESSAGES_HPP

#include "byte_buffer.hpp"

namespace networking
{

namespace messages
{

struct get_chunk
{
	unsigned int ld_x, ld_y, ru_x, ru_y;

	void serialize_to_buffer(serialization::byte_buffer &buffer) const
	{
		buffer.put_unsigned_int(ld_x);
		buffer.put_unsigned_int(ld_y);
		buffer.put_unsigned_int(ru_x);
		buffer.put_unsigned_int(ru_y);
	}

	void deserialize_from_buffer(serialization::byte_buffer &buffer)
	{
		ld_x = buffer.get_unsigned_int();
		ld_y = buffer.get_unsigned_int();
		ru_x = buffer.get_unsigned_int();
		ru_y = buffer.get_unsigned_int();
	}

	static char message_id()
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

	static char message_id()
	{
		return 1;
	}

	std::string content;
};

struct position_changed
{
	int player_id, old_x, old_y, new_x, new_y;

	void serialize_to_buffer(serialization::byte_buffer &buffer) const
	{
		buffer.put_int(player_id);
		buffer.put_int(old_x);
		buffer.put_int(old_y);
		buffer.put_int(new_x);
		buffer.put_int(new_y);
	}

	void deserialize_from_buffer(serialization::byte_buffer &buffer)
	{
		player_id = buffer.get_int();
		old_x = buffer.get_int();
		old_y = buffer.get_int();
		new_x = buffer.get_int();
		new_y = buffer.get_int();
	}

	static char message_id()
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

	static char message_id()
	{
		return 3;
	}

	std::string content {"OK"};
};

struct get_enemies_data
{
	get_enemies_data() = default;

	void serialize_to_buffer(serialization::byte_buffer &buffer) const
	{
		buffer.put_string(content);
	}

	void deserialize_from_buffer(serialization::byte_buffer &buffer)
	{
		content = buffer.get_string();
	}

	static char message_id()
	{
		return 4;
	}
	std::string content{"enemies"};
};

struct get_enemies_data_response
{
	get_enemies_data_response() = default;

	get_enemies_data_response(const std::vector<int> &enemies_data)
		: content(enemies_data)
	{
	}

	void serialize_to_buffer(serialization::byte_buffer &buffer) const
	{
		buffer.put_int_vector(content);
	}

	void deserialize_from_buffer(serialization::byte_buffer &buffer)
	{
		content = buffer.get_int_vector();
	}

	static char message_id()
	{
		return 5;
	}
	std::vector<int> content; //[id, posx, posy]
};

struct get_players_data
{
	get_players_data() = default;

	void serialize_to_buffer(serialization::byte_buffer &buffer) const
	{
		buffer.put_string(content);
	}

	void deserialize_from_buffer(serialization::byte_buffer &buffer)
	{
		content = buffer.get_string();
	}

	static char message_id()
	{
		return 6;
	}
	std::string content {"players"};
};

struct get_players_data_response
{
	get_players_data_response() = default;

	get_players_data_response(const std::vector<int> &players_data)
		: content(players_data)
	{
	}

	void serialize_to_buffer(serialization::byte_buffer &buffer) const
	{
		buffer.put_int_vector(content);
	}

	void deserialize_from_buffer(serialization::byte_buffer &buffer)
	{
		content = buffer.get_int_vector();
	}

	static char message_id()
	{
		return 7;
	}

	std::vector<int> content; //[id, posx, posy]
};

struct client_shutdown
{
	client_shutdown() = default;

	void serialize_to_buffer(serialization::byte_buffer &buffer) const
	{
		buffer.put_int(player_id);
	}

	void deserialize_from_buffer(serialization::byte_buffer &buffer)
	{
		player_id = buffer.get_int();
	}

	static char message_id()
	{
		return 8;
	}

	int player_id;
};

struct get_id
{
	get_id() = default;

	void serialize_to_buffer(serialization::byte_buffer &buffer) const
	{
		buffer.put_string(content);
	}

	void deserialize_from_buffer(serialization::byte_buffer &buffer)
	{
		content = buffer.get_string();
	}

	static char message_id()
	{
		return 9;
	}
	std::string content {"id"};
};

struct get_id_response
{
	get_id_response() = default;

	void serialize_to_buffer(serialization::byte_buffer &buffer) const
	{
		buffer.put_int(player_id);
	}

	void deserialize_from_buffer(serialization::byte_buffer &buffer)
	{
		player_id = buffer.get_int();
	}

	static char message_id()
	{
		return 10;
	}
	int player_id;
};

}

}

#endif // MESSAGES_HPP
