#include "HasQuantity.h"

#include "../error/NotSufficientError.h"

unsigned int HasQuantity::getQuantity() const {
    return quantity;
}

void HasQuantity::fill(const unsigned int number) {
    quantity += number;
}

void HasQuantity::draw(const unsigned int number) {
    if (number > quantity) {
        throw NotSufficientError("Not enough items");
    }

    quantity -= number;
}
