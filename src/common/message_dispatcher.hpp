#ifndef MESSAGE_DISPATCHER_HPP
#define MESSAGE_DISPATCHER_HPP

#include <cassert>
#include <string>
#include <functional>
#include <vector>
#include <map>
#include <experimental/optional>

#include "logger.hpp"
#include "byte_buffer.hpp"

namespace networking
{

/*
 * Ok. This is the most crazy implementation I have ever worked with.
       Ref: http://stackoverflow.com/questions/25714390/is-it-possible-to-do-this-lambda-event-manager-in-c
 * 1. boost::any
     - holds any type. Sth like dynamic in C#.
     - It's impossible to implicit cast from boost::any. We must use boost::any_cast
     - It's not implemented by void*! It uses dynamic polymorphism and virtual methods instead.
   2. Observing callstack is good idea to understanding how it works
 */

template<class T>
using optional = std::experimental::optional<T>;

using dispatcher_type = std::function<optional<serialization::byte_buffer>
                                                    (serialization::byte_buffer &)>;

template<typename T>
struct function_traits;

template<typename R, typename C, typename Arg>
struct function_traits<R(C::*)(Arg)>
{
    using return_type = R;
    using arg_type = Arg;
};

template<typename R, typename C, typename Arg>
struct function_traits<R(C::*)(Arg) const>
{
    using return_type = R;
    using arg_type = Arg;
};


template<typename Ret, typename Arg>
struct dispatcher_maker;

template<typename Ret, typename Arg>
struct dispatcher;


template<typename Ret, typename Arg>
struct dispatcher_maker
{
    template<typename F>
    dispatcher_type make(F&& f)
    {
        return dispatcher<Ret, Arg>{std::forward<F>(f)};
    }
};

template<typename F>
dispatcher_type make_dispatcher(F&& f) // add dispatcher_type
{
    using f_type = decltype(&F::operator());
    using arg_type = typename function_traits<f_type>::arg_type;
    using return_type = typename function_traits<f_type>::return_type;

    return dispatcher_maker<return_type, arg_type>().make(std::forward<F>(f));
}

// concept for handlers, that can't be void??

template<typename Ret, typename Arg>
struct dispatcher
{
    constexpr static int sizeof_msg_size = sizeof(unsigned short);

    template<typename F> dispatcher(F f) : handler(std::move(f)) { }

    optional<serialization::byte_buffer> operator()(serialization::byte_buffer &buffer)
    {
        using Req = typename std::remove_reference<Arg>::type;
        using Resp = typename std::remove_reference<Ret>::type;
        int id = buffer.m_byte_buffer[buffer.offset];

        if (id == Req::message_id())
        {
            // msg = it's request here
            Req req;
            buffer.offset++;
            req.deserialize_from_buffer(buffer);

            Resp resp = handler(req);

            serialization::byte_buffer data;
            data.put_unsigned_short(0);
            data.put_char(resp.message_id());
            resp.serialize_to_buffer(data);

            assert(data.get_size() >= sizeof_msg_size);
            assert(data.get_size() - sizeof_msg_size < 256*256);
            unsigned short size = (unsigned short)(data.get_size() - sizeof_msg_size);
            memcpy(&data.m_byte_buffer[0], &size, sizeof(size));
            return data;
        }
        return {};
    }

private:
    std::function<Ret(Arg)> handler;
};


/*
 * be aware that there is no implicit conversion so 1.0 is double but 1.0f is float and "dupa"
   is c-string
 * static variables don't have to be captured by lambda!
 */
struct message_dispatcher
{
public:

    template<typename F>
    void add_handler(F&& f)
    {
        callbacks.emplace_back(make_dispatcher(std::forward<F>(f)));
    }

    serialization::byte_buffer dispatch_req_get_resp(serialization::byte_buffer &buffer)
    {
        return dispatch(buffer);
    }

private:
    serialization::byte_buffer dispatch(serialization::byte_buffer &buffer)
    {
        for (auto some_dispatcher : callbacks)
        {
            auto maybe_data = call(some_dispatcher, buffer);
            if (maybe_data)
                return *maybe_data;
        }
        assert(false); // no handler
    }

    template<typename Dispatcher>
    optional<serialization::byte_buffer> call(Dispatcher const& dispatcher,
                                              serialization::byte_buffer &buffer)
    {
        return dispatcher(buffer);
    }

private:
    std::vector<dispatcher_type> callbacks;
};

}

#endif // MESSAGE_DISPATCHER_HPP
