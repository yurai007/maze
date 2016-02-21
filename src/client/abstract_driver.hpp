#ifndef ABSTRACT_DRIVER_HPP
#define ABSTRACT_DRIVER_HPP

#include <string>

class abstract_driver
{
public:
    virtual int run(const std::string &ip_address) = 0;
    virtual ~abstract_driver() = default;
};

#endif // ABSTRACT_DRIVER_HPP
