#include <stdexcept>

#ifndef VENDINGMACHINE_LOCKERROR_H
#define VENDINGMACHINE_LOCKERROR_H

class LockError : public std::logic_error {
public:
    explicit LockError(const std::string &what_arg)
        : std::logic_error(what_arg) {}
};

#endif //VENDINGMACHINE_LOCKERROR_H
