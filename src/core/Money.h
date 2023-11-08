#include <iomanip>
#include <iostream>

#ifndef VENDINGMACHINE_MONEY_H
#define VENDINGMACHINE_MONEY_H

class Money {
public:
    Money(long long int cents) // NOLINT(*-explicit-constructor)
        : cents{cents} {}

    Money operator+(const Money &rhs) const;

    Money operator-(const Money &rhs) const;

    Money &operator+=(const Money &rhs);

    Money &operator-=(const Money &rhs);

    operator long long int() const; // NOLINT(*-explicit-constructor)

    long long int getValue() const;

    friend std::ostream& operator<<(std::ostream& os, const Money& money);

private:
    long long int cents;
};

#endif //VENDINGMACHINE_MONEY_H
