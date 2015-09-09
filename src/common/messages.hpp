#ifndef MESSAGES_HPP
#define MESSAGES_HPP

#include "byte_buffer.hpp"

// TO DO: problem because message_id sizeof is 1 in serialization

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

	static int message_id()
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

	static int message_id()
	{
		return 5;
	}
	std::vector<int> content; //[id, posx, posy]
};

}

}

#endif // MESSAGES_HPP
