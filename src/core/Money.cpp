#include "Money.h"

#include <iomanip>
#include <sstream>

Money Money::operator+(const Money &rhs) const {
    return cents + rhs.cents;
}

Money Money::operator-(const Money &rhs) const {
    return cents - rhs.cents;
}

Money Money::operator*(const long long& rhs) const {
    return cents * rhs;
}

Money &Money::operator+=(const Money &rhs) {
    cents += rhs.cents;
    return *this;
}

Money &Money::operator-=(const Money &rhs) {
    cents -= rhs.cents;
    return *this;
}

Money& Money::operator*=(const long long& rhs) {
    cents *= rhs;
    return *this;
}

Money::operator long long int() const {
    return cents;
}

Money::operator std::string() const {
    const long long int yuan = cents / 100;
    const long long int fen = cents % 100;

    std::ostringstream os;
    os << "￥" << yuan << '.'
        << std::setw(2) << std::setfill('0') << fen
        << std::setw(0) << std::setfill(' ');

    return os.str();
}

long long int Money::getValue() const {
    return cents;
}
