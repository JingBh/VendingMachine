#include "Money.h"

Money Money::operator+(const Money &rhs) const {
    return cents + rhs.cents;
}

Money Money::operator-(const Money &rhs) const {
    return cents - rhs.cents;
}

Money &Money::operator+=(const Money &rhs) {
    cents += rhs.cents;
    return *this;
}

Money &Money::operator-=(const Money &rhs) {
    cents -= rhs.cents;
    return *this;
}

Money::operator long long int() const {
    return cents;
}

long long int Money::getValue() const {
    return cents;
}
