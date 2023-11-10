#ifndef VENDINGMACHINE_MONEY_H
#define VENDINGMACHINE_MONEY_H

#include <nlohmann/json.hpp>

class Money {
public:
    // ReSharper disable once CppNonExplicitConvertingConstructor
    Money(const long long int cents) // NOLINT(*-explicit-constructor)
        : cents{cents} {}

    Money operator+(const Money &rhs) const;

    Money operator-(const Money &rhs) const;

    Money &operator+=(const Money &rhs);

    Money &operator-=(const Money &rhs);

    // ReSharper disable once CppNonExplicitConversionOperator
    operator long long int() const; // NOLINT(*-explicit-constructor)

    long long int getValue() const;

private:
    long long int cents;
};

template<>
struct nlohmann::adl_serializer<Money> {
    static void to_json(json &j, const Money &obj) {
        j = obj.getValue();
    }

    static Money from_json(const json &j) {
        return j.get<long long int>();
    }
};

#endif //VENDINGMACHINE_MONEY_H
