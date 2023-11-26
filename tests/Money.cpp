#include <string>

#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>

#include "../src/core/Money.h"

TEST_CASE("Money", "[Money]") {

    SECTION("Construction and coversion") {
        const Money money{100};

        CHECK(money.getValue() == 100);
        CHECK(static_cast<Money>(100).getValue() == 100);
        CHECK(static_cast<long long int>(money) == 100);
    }

    SECTION("Calculation") {
        Money money1{123};
        Money money2{234};
        Money money3{345};

        REQUIRE(money1 + money2 == 357);
        REQUIRE(money2 - money1 == 111);
        REQUIRE(money1.operator*(2) == 246);
        REQUIRE((money1 += 20) == 143);
        REQUIRE((money2 -= 20) == 214);
        REQUIRE((money3 *= 2) == 690);
    }

    SECTION("Formatting") {
        const Money money1{0};
        CHECK(static_cast<std::string>(money1) == "￥0.00");

        const Money money2{88};
        CHECK(static_cast<std::string>(money2) == "￥0.88");

        const Money money3{1234};
        CHECK(static_cast<std::string>(money3) == "￥12.34");
    }

    SECTION("Serialization") {
        const Money money1{100};

        const nlohmann::json json = money1;
        REQUIRE(json.dump() == "100");

        const Money money2 = json.get<Money>();
        REQUIRE(money2.getValue() == 100);
    }

}
