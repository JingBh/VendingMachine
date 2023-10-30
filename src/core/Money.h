#include <iostream>
#include <iomanip>

#ifndef VENDINGMACHINE_MONEY_H
#define VENDINGMACHINE_MONEY_H

class Money {
public:
    Money(long long int cents) // NOLINT(*-explicit-constructor)
        : cents{cents} {}

    long long int getValue() const;

    operator long long int() const; // NOLINT(*-explicit-constructor)

    friend std::ostream& operator<<(std::ostream& os, const Money& money);

private:
    const long long int cents;
};

#endif //VENDINGMACHINE_MONEY_H
