#include <stdexcept>

#include "HasQuantity.h"

unsigned int HasQuantity::getQuantity() const {
    return quantity;
}

void HasQuantity::fill(unsigned int number = 1) {
    quantity += number;
}

void HasQuantity::draw(unsigned int number = 1) {
    if (number > quantity) {
        throw std::invalid_argument("not enough items");
    }

    quantity -= number;
}
