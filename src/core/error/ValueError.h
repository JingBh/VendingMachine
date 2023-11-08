#include <stdexcept>
#include <string>

#ifndef VENDINGMACHINE_VALUEERROR_H
#define VENDINGMACHINE_VALUEERROR_H

class ValueError : public std::runtime_error {
public:
    explicit ValueError(const std::string &msg)
        : std::runtime_error(msg) {}
};

#endif //VENDINGMACHINE_VALUEERROR_H
