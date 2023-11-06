#include <stdexcept>

#ifndef VENDINGMACHINE_NOTSUFFICIENTERROR_H
#define VENDINGMACHINE_NOTSUFFICIENTERROR_H

class NotSufficientError : public std::logic_error {
public:
    explicit NotSufficientError(const std::string &what_arg)
        : std::logic_error(what_arg) {}
};

class MoneyNotSufficientError : public NotSufficientError {
public:
    explicit MoneyNotSufficientError(const std::string &what_arg)
        : NotSufficientError(what_arg) {}
};

class OutOfStockError : public NotSufficientError {
public:
    explicit OutOfStockError(const std::string &what_arg)
        : NotSufficientError(what_arg) {}
};

#endif //VENDINGMACHINE_NOTSUFFICIENTERROR_H
