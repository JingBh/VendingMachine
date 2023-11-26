#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>

#include "../src/core/Good.h"

TEST_CASE("Good", "[Good]") {

    SECTION("Data") {
        const auto goodCocaCola = Good{COCA_COLA};
        CHECK(goodCocaCola.getName() == "可口可乐");
        CHECK(goodCocaCola.getPrice() == 200);

        const auto goodPepsiCola = Good{PEPSI_COLA};
        CHECK(goodPepsiCola.getName() == "百事可乐");
        CHECK(goodPepsiCola.getPrice() == 200);

        const auto goodOrangeJuice = Good{ORANGE_JUICE};
        CHECK(goodOrangeJuice.getName() == "橙汁");
        CHECK(goodOrangeJuice.getPrice() == 300);

        const auto goodCoffee = Good{COFFEE};
        CHECK(goodCoffee.getName() == "咖啡");
        CHECK(goodCoffee.getPrice() == 500);

        const auto goodWater = Good{WATER};
        CHECK(goodWater.getName() == "纯净水");
        CHECK(goodWater.getPrice() == 150);
    }

    SECTION("Quantity") {
        Good good1{COCA_COLA};

        // default quantity is 1
        REQUIRE(good1.getQuantity() == 1);

        // default fill is +1
        good1.fill();
        REQUIRE(good1.getQuantity() == 2);

        // default draw is -1
        good1.draw();
        REQUIRE(good1.getQuantity() == 1);

        // quantity can't be negative
        REQUIRE_THROWS(good1.draw(2));

        Good good2{COCA_COLA, 0};
        REQUIRE_THROWS(good2.draw());
    }

    SECTION("Comparison") {
        const Good good1{COCA_COLA};
        const Good good2{PEPSI_COLA};
        const Good good3{COCA_COLA};

        REQUIRE(good1.isOfSameType(good2) == false);
        REQUIRE(good1.isOfSameType(good3) == true);
    }

    SECTION("Serialization") {
        const Good good1{COFFEE, 10};

        const nlohmann::json json = good1;
        REQUIRE(json.dump() == "{\"quantity\":10,\"type\":3}");

        const Good good2 = json.get<Good>();
        REQUIRE(good2.getType() == COFFEE);
        REQUIRE(good2.getQuantity() == 10);
    }

}
