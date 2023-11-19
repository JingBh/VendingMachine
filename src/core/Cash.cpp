#include "Cash.h"

Money Cash::getFaceValue() const {
    return value;
}

Money Cash::getTotalValue() const {
    return value.operator*(quantity);
}

Money Cash::valueOf(const CashType cashType) {
    switch (cashType) {
        case TEN_YUAN:
            return 1000;

        case FIVE_YUAN:
            return 500;

        case TWO_YUAN:
            return 200;

        case ONE_YUAN:
            return 100;

        case FIFTY_CENTS:
            return 50;

        default:
            return 0;
    }
}
