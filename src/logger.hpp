#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <cstdio>

namespace framework
{

class logger
{
public:

    static logger& get()
    {
        static logger instance(true, true, true);
        return instance;
    }

    void log(const char *string, ...);

private:

    logger(bool enabled, bool log_to_file, bool log_date);

    template<class Msg>
    void log_message(const Msg &msg)
    {
        if (!on)
            return;
        log("Message: {name = \"%s\", id = %d}", msg.name.c_str(), msg.id);
    }

    ~logger();

    char *get_time();

    bool on, write_to_file, write_date;
    static const int max_log_size = 128;
    char buffer[max_log_size];
    FILE * file_proxy = nullptr;
};

}

#endif // LOGGER_HPP
