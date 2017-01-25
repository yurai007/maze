#ifndef MESSAGES_HPP
#define MESSAGES_HPP

#include <boost/mpl/vector.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/for_each.hpp>

#include "byte_buffer.hpp"

namespace networking
{

namespace messages
{

struct get_chunk;
struct get_chunk_response;
struct position_changed;
struct position_changed_response;
struct client_shutdown;
struct get_id;
struct get_id_response;
struct get_resources_data;
struct get_resources_data_response;
struct fireball_triggered;
struct fireball_triggered_response;

using registered_messages = boost::mpl::vector<get_chunk, get_chunk_response, position_changed,
    position_changed_response, client_shutdown, get_id, get_id_response, get_resources_data,
    get_resources_data_response, fireball_triggered, fireball_triggered_response>;

template <class T>
struct message_numerator
{
    static char message_id()
    {
        using iter = typename boost::mpl::find<registered_messages, T>::type;
        static_assert(iter::pos::value < boost::mpl::size<registered_messages>::value,
                      "message is NOT registered");
        return (char)iter::pos::value;
    }
};

struct get_chunk : public message_numerator<get_chunk>
{
    unsigned ld_x, ld_y, ru_x, ru_y;

    get_chunk() = default;
    get_chunk(unsigned ld_xx, unsigned ld_yy, unsigned ru_xx, unsigned ru_yy) :
        ld_x(ld_xx), ld_y(ld_yy), ru_x(ru_xx), ru_y(ru_yy) {}

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
};

struct get_chunk_response : public message_numerator<get_chunk_response>
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

	std::string content;
};

struct position_changed : public message_numerator<position_changed>
{
	int player_id, old_x, old_y, new_x, new_y;

    position_changed() = default;
    position_changed(int player_id_, int old_xx, int old_yy, int new_xx, int new_yy)
        : player_id(player_id_), old_x(old_xx), old_y(old_yy), new_x(new_xx), new_y(new_yy) {}

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

};

struct position_changed_response : public message_numerator<position_changed_response>
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

	std::string content {"OK"};
};

struct client_shutdown : public message_numerator<client_shutdown>
{
	client_shutdown() = default;
    client_shutdown(int player_id_) : player_id(player_id_) {}

	void serialize_to_buffer(serialization::byte_buffer &buffer) const
	{
		buffer.put_int(player_id);
	}

	void deserialize_from_buffer(serialization::byte_buffer &buffer)
	{
		player_id = buffer.get_int();
	}

	int player_id;
};

struct get_id : public message_numerator<get_id>
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

	std::string content {"id"};
};

struct get_id_response : public message_numerator<get_id_response>
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

	int player_id;
};

struct get_resources_data : public message_numerator<get_resources_data>
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

    std::string content {"resources"};
};

struct get_resources_data_response : public message_numerator<get_resources_data_response>
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

    std::vector<int> content; //[type, posx, posy]
};

struct fireball_triggered : public message_numerator<fireball_triggered>
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
};

struct fireball_triggered_response : public message_numerator<fireball_triggered_response>
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
    std::string content {"OK"};
};



struct verify
{
    template<typename Msg> void operator()(Msg)
    {
        assert((unsigned)Msg::message_id() == counter);
        counter++;
    }
    static unsigned counter;
};

extern void verify_messages();

}

}

#endif // MESSAGES_HPP
