#include "Cash.h"

Money Cash::getFaceValue() const {
    return value;
}

Money Cash::getTotalValue() const {
    return value * quantity;
}
