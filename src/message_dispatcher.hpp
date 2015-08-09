#ifndef MESSAGE_DISPATCHER_HPP
#define MESSAGE_DISPATCHER_HPP

#include <memory>
#include <cassert>
#include <string>
#include <functional>
#include <vector>
#include <boost/any.hpp>
#include <map>

#include "remote_transport.hpp"
#include "logger.hpp"

namespace networking
{

/*
 * Ok. This is the most crazy implementation I have ever worked with.
       Ref: http://stackoverflow.com/questions/25714390/is-it-possible-to-do-this-lambda-event-manager-in-c
 * 1. boost::any
     - holds any type. Sth like dynamic in C#.
     - It's impossible to implicit cast form boost::any. We must use boost::any_cast
     - It's not implemented by void*! It uses dynamic polymorphism and virtual methods instead.
   2. Observing callstack is good idea to understanding how it works
 */

typedef std::function<bool(std::vector<boost::any> const&)> dispatcher_type;

// Remains those traits to understand

template<typename T>
struct function_traits;

template<typename R, typename C, typename... Args>
struct function_traits<R(C::*)(Args...)>
{
    using args_type = std::tuple<Args...>;
};

template<typename R, typename C, typename... Args>
struct function_traits<R(C::*)(Args...) const>
{
    using args_type = std::tuple<Args...>;
};


template<typename T>
struct dispatcher_maker;

template<typename Arg>
struct dispatcher;

/* It converts somehow dispatcher to dispatcher_type.
   So it converts functor (with operator()) to std::function<void()> const&)>
*/
template<typename Arg>
struct dispatcher_maker<std::tuple<Arg>>
{
    template<typename F>
    dispatcher_type make(F&& f)
    {
        return dispatcher<Arg>{std::forward<F>(f)};
    }
};

template<typename F>
std::function<bool(std::vector<boost::any> const&)> make_dispatcher(F&& f)
{
    using f_type = decltype(&F::operator());

    using args_type = typename function_traits<f_type>::args_type;

    return dispatcher_maker<args_type>{}.make(std::forward<F>(f));
}

template<typename Arg>
struct dispatcher
{
    template<typename F> dispatcher(F f) : _f(std::move(f)) { }
    bool operator () (std::vector<boost::any> const& v)
    {
        return do_call(v);
    }
private:

    bool do_call(std::vector<boost::any> const& v)
    {
        try
        {
            _f(boost::any_cast<Arg>(v[0]));
        }
        catch (boost::bad_any_cast const&)
        {
            return false;
        }
        return true;
    }

    std::function<void(Arg)> _f;
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

    template<typename Arg>
    void dispatch(Arg const& arg)
    {
        for (auto some_dispatcher : callbacks)
        {
            if (call(some_dispatcher, arg))
                return;
        }
        logger_.log("message dispatcher: there is no handler for this msg");
    }

private:

    template<typename F, typename Arg>
    bool call(F const& f, Arg const& arg)
    {
        std::vector<boost::any> v{arg};
        return f(v);
    }

private:
    std::vector<dispatcher_type> callbacks;
};

}

#endif // MESSAGE_DISPATCHER_HPP
