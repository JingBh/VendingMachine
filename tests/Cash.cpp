#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>

#include "../src/core/Cash.h"

TEST_CASE("Cash", "[Cash]") {

    SECTION("Data") {
        CHECK(Cash::valueOf(TEN_YUAN) == 1000);
        CHECK(Cash::valueOf(FIVE_YUAN) == 500);
        CHECK(Cash::valueOf(TWO_YUAN) == 200);
        CHECK(Cash::valueOf(ONE_YUAN) == 100);
        CHECK(Cash::valueOf(FIFTY_CENTS) == 50);
    }

    SECTION("Value") {
        const Cash good1{TEN_YUAN};
        CHECK(good1.getFaceValue() == 1000);
        CHECK(good1.getTotalValue() == 1000);

        const Cash good2{FIVE_YUAN, 4};
        CHECK(good2.getFaceValue() == 500);
        CHECK(good2.getTotalValue() == 2000);
    }

    SECTION("Serialization") {
        const Cash cash1{FIVE_YUAN, 10};

        const nlohmann::json json = cash1;
        REQUIRE(json.dump() == "{\"quantity\":10,\"value\":500}");

        const Cash cash2 = json.get<Cash>();
        REQUIRE(cash2.getFaceValue() == 500);
        REQUIRE(cash2.getQuantity() == 10);
    }

}
