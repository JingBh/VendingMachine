#ifndef VENDINGMACHINE_GOOD_H
#define VENDINGMACHINE_GOOD_H

#include <string>

#include <nlohmann/json.hpp>

#include "contract/HasQuantity.h"
#include "Money.h"

enum GoodType {
    COCA_COLA,
    PEPSI_COLA,
    ORANGE_JUICE,
    COFFEE,
    WATER
};

class Good final : public HasQuantity {
public:
    explicit Good(const GoodType type)
        : type{type} {}

    Good(const GoodType type, const unsigned int quantity)
        : type{type}, HasQuantity{quantity} {}

    GoodType getType() const;

    std::string getImageId() const;

    std::string getName() const;

    Money getPrice() const;

    bool isOfSameType(const Good &other) const;

private:
    const GoodType type;
};


template<>
struct nlohmann::adl_serializer<Good> {
    static void to_json(json &j, const Good &obj) {
        j = json{
            { "type", obj.getType() },
            { "quantity", obj.getQuantity() }
        };
    }

    static Good from_json(const json &j) {
        return Good{
            j.at("type").get<GoodType>(),
            j.at("quantity").get<unsigned int>()
        };
    }
};

#endif //VENDINGMACHINE_GOOD_H
