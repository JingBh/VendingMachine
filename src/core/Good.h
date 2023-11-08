#include <string>

#include "contract/HasQuantity.h"
#include "Money.h"

#ifndef VENDINGMACHINE_GOOD_H
#define VENDINGMACHINE_GOOD_H

enum GoodType {
    COCA_COLA,
    PEPSI_COLA,
    ORANGE_JUICE,
    COFFEE,
    WATER
};

class Good : public HasQuantity {
public:
    explicit Good(GoodType type)
        : type{type} {}

    Good(GoodType type, unsigned int quantity)
        : type{type}, HasQuantity{quantity} {}

    GoodType getType() const;

    std::string getName() const;

    Money getPrice() const;

    bool isOfSameType(const Good &other) const;

private:
    const GoodType type;
};

#endif //VENDINGMACHINE_GOOD_H
