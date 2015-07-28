#ifndef MESSAGE_DISPATCHER_HPP
#define MESSAGE_DISPATCHER_HPP

#include <memory>

#include "remote_transport.hpp"

namespace networking
{

namespace messages
{
	struct get_chunk
	{
		int ld_x, ld_y, ru_x, ru_y;
	};

	struct get_chunk_response
	{
	};

	struct position_changed
	{
		int old_x, old_y, new_x, new_y;
	};

	struct position_changed_response
	{
	};
}

namespace dispatching
{

struct last_msg
{
	static char message_id()
	{
		return 0;
	}
};

template<typename previous_dispatcher, typename Msg, typename Func>
class template_dispatcher
{
	std::shared_ptr<remote_transport::receiver> receiver;
	previous_dispatcher* prev;
	Func handler;
	bool chained;

	template_dispatcher(template_dispatcher const&) = delete;
	template_dispatcher& operator=(template_dispatcher const&) = delete;

	template<typename Dispatcher, typename OtherMsg, typename OtherFunc> friend class template_dispatcher;

	void wait_and_dispatch()
	{
		receiver->wait_on_msg();
		dispatch();
	}

	bool dispatch()
	{
		char id = Msg::message_id();
		if (receiver->received_msg_with_type(id))
		{
			Msg msg;
			receiver->deserialize_from_buffer(msg);
			handler(msg);
			return true;
		}
		else
			return prev->dispatch();
	}

public:

	template_dispatcher(template_dispatcher&& other) :
		receiver(other.receiver),
		prev(other.prev),
		handler(std::move(other.handler)),
		chained(other.chained)
	{
		other.chained = true;
	}

	template_dispatcher(std::shared_ptr<remote_transport::receiver> preceiver,
						previous_dispatcher* prev_, Func&& handler_) :
		receiver(preceiver),
		prev(prev_),
		handler(std::forward<Func>(handler_)),
		chained(false)
	{
		prev_->chained = true;
	}

	template<typename OtherMsg, typename OtherFunc>
	template_dispatcher<template_dispatcher, OtherMsg, OtherFunc> handle(OtherFunc&& handler_)
	{
		return template_dispatcher<template_dispatcher, OtherMsg, OtherFunc>(
			receiver, this, std::forward<OtherFunc>(handler_));
	}

	~template_dispatcher()
	{
		if (!chained)
			wait_and_dispatch();
	}
};



class dispatcher
{
	std::shared_ptr<remote_transport::receiver> receiver;
	bool chained;

	dispatcher(dispatcher const&) = delete;
	dispatcher& operator=(dispatcher const&) = delete;

	template<typename Dispatcher, typename Msg, typename Func> friend class template_dispatcher;

	void wait_and_dispatch()
	{
		receiver->wait_on_msg();
		dispatch();
	}

	bool dispatch()
	{
		char id = last_msg::message_id();
		if (receiver->received_msg_with_type(id))
		{
			assert(false);
		}
		return false;
	}

public:
	dispatcher(dispatcher&& other) :
		receiver(other.receiver),
		chained(other.chained)
	{
		other.chained = true;
	}

	explicit dispatcher(std::shared_ptr<remote_transport::receiver> preceiver) :
		receiver(preceiver),
		chained(false)
	{
	}

	template<typename Message, typename Func>
	template_dispatcher<dispatcher, Message, Func> handle(Func&& handler)
	{
		return template_dispatcher<dispatcher, Message, Func>(
			receiver, this, std::forward<Func>(handler));
	}

	~dispatcher()
	{
		if (!chained)
			wait_and_dispatch();
	}
};

}

}

#endif // MESSAGE_DISPATCHER_HPP
