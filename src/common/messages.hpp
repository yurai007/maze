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

	get_enemies_data(int player_id_)
		: player_id(player_id_)
	{
	}

	void serialize_to_buffer(serialization::byte_buffer &buffer) const
	{
		buffer.put_string(content);
		buffer.put_int(player_id);
	}

	void deserialize_from_buffer(serialization::byte_buffer &buffer)
	{
		content = buffer.get_string();
		player_id = buffer.get_int();
	}

	static char message_id()
	{
		return 4;
	}
	std::string content{"enemies"};
	int player_id;
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

struct get_resources_data
{
    get_resources_data() = default;

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
        return 11;
    }
    std::string content {"resources"};
};

struct get_resources_data_response
{
    get_resources_data_response() = default;

    get_resources_data_response(const std::vector<int> &resources_data)
        : content(resources_data)
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
        return 12;
    }

    std::vector<int> content; //[type, posx, posy]
};

struct fireball_triggered
{
    int player_id, pos_x, pos_y;
    char direction;

    void serialize_to_buffer(serialization::byte_buffer &buffer) const
    {
        buffer.put_int(player_id);
        buffer.put_int(pos_x);
        buffer.put_int(pos_y);
        buffer.put_char(direction);
    }

    void deserialize_from_buffer(serialization::byte_buffer &buffer)
    {
        player_id = buffer.get_int();
        pos_x = buffer.get_int();
        pos_y = buffer.get_int();
        direction = buffer.get_char();
    }

    // hey, this ugly expicit numbers may be for sure replaced by automatic metaprogramming
    // some kind of type container + size of this container?
    static char message_id()
    {
        return 13;
    }
};

struct fireball_triggered_response
{
    fireball_triggered_response() = default;

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
        return 14;
    }

    std::string content {"OK"};
};

}

}

#endif // MESSAGES_HPP
