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
        : HasQuantity{quantity}, value{valueOf(cashType)} {}

    explicit Cash(const Money value)
        : value{value} {}

    Cash(const Money value, const unsigned int quantity)
        : HasQuantity{quantity}, value{value} {}

    Cash(const Cash &other)
        : HasQuantity{other.quantity}, value{other.value} {}

    Cash(Cash &&other) noexcept
        : HasQuantity{other.quantity}, value{other.value} {}

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
