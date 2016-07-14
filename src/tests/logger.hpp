#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <cstdio>
#include <mutex>

namespace framework
{

/*
 * logger is singleton so protection is needed only in log method

 * delegating variadic function in C is painful:
   http://stackoverflow.com/questions/150543/forward-an-invocation-of-a-variadic-function-in-c
   I must every time declar va_list, do va_start/va_end.
   I decided to use variadic templates for public methods and elipsis for delegation to vsnprintf
*/

class logger
{
public:

    static logger& get()
    {
        static logger instance(true, true, true, false);
        return instance;
    }

    template<typename... Arg>
    void log(const char *string, Arg... args)
    {
        internal_log(false, string, args...);
    }

    template<typename... Arg>
    void log_debug(const char *string, Arg... args)
    {
        if (debug)
            internal_log(false, string, args...);
    }

    template<typename... Arg>
    void log_in_place(const char *string, Arg... args)
    {
        internal_log(true, string, args...);
    }

    void enable(bool enabled);

private:

    logger(bool enabled, bool log_to_file, bool log_date, bool debug_on);

    template<class Msg>
    void log_message(const Msg &msg)
    {
        log("Message: {name = \"%s\", id = %d}", msg.name.c_str(), msg.id);
    }

    ~logger();

    char *put_time_in_buffer();
    void internal_log(bool in_place, const char *string, ...);

    bool on;
    const bool write_to_file, write_date, debug;
    bool opened {false};
    constexpr static int max_line_size = 128;
    char buffer[max_line_size];
    FILE * file_proxy = nullptr;
    std::mutex mutex;
};

}

#define logger_ framework::logger::get()

#endif // LOGGER_HPP
