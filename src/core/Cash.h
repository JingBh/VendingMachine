#ifndef VENDINGMACHINE_CASH_H
#define VENDINGMACHINE_CASH_H

#include <nlohmann/json.hpp>

#include "contract/HasQuantity.h"
#include "Money.h"

enum CashType {
    TEN_YUAN,
    FIVE_YUAN,
    TWO_YUAN,
    ONE_YUAN,
    FIFTY_CENTS
};

class Cash final : public HasQuantity {
public:
    explicit Cash(const CashType cashType)
        : value{valueOf(cashType)} {}

    Cash(const CashType cashType, const unsigned int quantity)
        : value{valueOf(cashType)}, HasQuantity{quantity} {}

    explicit Cash(const Money value)
        : value{value} {}

    Cash(const Money value, const unsigned int quantity)
        : value{value}, HasQuantity{quantity} {}

    Cash(const Cash &other)
        : value{other.value}, HasQuantity(other.quantity) {}

    Cash(Cash &&other) noexcept
        : value{other.value}, HasQuantity(other.quantity) {}

    Money getFaceValue() const;

    Money getTotalValue() const;

    static Money valueOf(CashType cashType);

private:
    const Money value;
};

template<>
struct nlohmann::adl_serializer<Cash> {
    static void to_json(json &j, const Cash &obj) {
        j = json{
            { "value", obj.getFaceValue() },
            { "quantity", obj.getQuantity() }
        };
    }

    static Cash from_json(const json &j) {
        return Cash{
            j.at("value").get<Money>(),
            j.at("quantity").get<unsigned int>()
        };
    }
};

#endif //VENDINGMACHINE_CASH_H
