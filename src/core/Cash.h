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

    Cash(const Cash &other)
        : value{other.value}, HasQuantity(other.quantity) {}

    Cash(Cash &&other) noexcept
        : value{other.value}, HasQuantity(other.quantity) {}

    Money getFaceValue() const;

    Money getTotalValue() const;

private:
    const Money value;

    static Money valueOf(CashType cashType);
};

#endif //VENDINGMACHINE_CASH_H
