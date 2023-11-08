#include "HasQuantity.h"

#include "../error/NotSufficientError.h"

unsigned int HasQuantity::getQuantity() const {
    return quantity;
}

void HasQuantity::fill(unsigned int number) {
    quantity += number;
}

void HasQuantity::draw(unsigned int number) {
    if (number > quantity) {
        throw NotSufficientError("Not enough items");
    }

    quantity -= number;
}
