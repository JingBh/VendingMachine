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

std::ostream &operator<<(std::ostream &os, const Money &money) {
    long long int yuan = money.cents / 100;
    long long int fen = money.cents % 100;

    os << "￥" << yuan << '.' << std::setw(2) << std::setfill('0') << fen;

    return os;
}
