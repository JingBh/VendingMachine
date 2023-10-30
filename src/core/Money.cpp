#include "Money.h"

long long int Money::getValue() const {
    return cents;
}

Money::operator long long int() const {
    return cents;
}

std::ostream &operator<<(std::ostream &os, const Money &money) {
    long long int yuan = money.cents / 100;
    long long int fen = money.cents % 100;

    os << "￥" << yuan << '.' << std::setw(2) << std::setfill('0') << fen;

    return os;
}
