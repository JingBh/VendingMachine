#include "contract/HasQuantity.h"
#include "Money.h"

#ifndef VENDINGMACHINE_CASH_H
#define VENDINGMACHINE_CASH_H

enum CashType {
    TEN_YUAN,
    FIVE_YUAN,
    TWO_YUAN,
    ONE_YUAN,
    FIFTY_CENTS
};

class Cash : public HasQuantity {
public:
    explicit Cash(CashType cashType)
        : value{valueOf(cashType)} {}

    Cash(CashType cashType, unsigned int quantity)
        : value{valueOf(cashType)}, HasQuantity{quantity} {}

    explicit Cash(Money value)
        : value{value} {}

    Cash(Money value, unsigned int quantity)
        : value{value}, HasQuantity{quantity} {}

    Money getFaceValue() const;

    Money getTotalValue() const;

private:
    const Money value;

    static Money valueOf(CashType cashType) {
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
};

#endif //VENDINGMACHINE_CASH_H
