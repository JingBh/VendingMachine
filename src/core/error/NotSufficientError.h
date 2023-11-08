#include <stdexcept>
#include <string>

#ifndef VENDINGMACHINE_NOTSUFFICIENTERROR_H
#define VENDINGMACHINE_NOTSUFFICIENTERROR_H

class NotSufficientError : public std::logic_error {
public:
    explicit NotSufficientError(const std::string &msg)
        : std::logic_error(msg) {}
};

class MoneyNotSufficientError : public NotSufficientError {
public:
    explicit MoneyNotSufficientError(const std::string &msg)
        : NotSufficientError(msg) {}
};

class OutOfStockError : public NotSufficientError {
public:
    explicit OutOfStockError(const std::string &msg)
        : NotSufficientError(msg) {}
};

#endif //VENDINGMACHINE_NOTSUFFICIENTERROR_H
