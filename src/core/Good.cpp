#include "Good.h"

std::string Good::getName() const {
    switch (type) {
        case COCA_COLA:
            return "可口可乐";

        case PEPSI_COLA:
            return "百事可乐";

        case ORANGE_JUICE:
            return "橙汁";

        case COFFEE:
            return "咖啡";

        case WATER:
            return "纯净水";

        default:
            return "Unknown Item";
    }
}

Money Good::getPrice() const {
    switch (type) {
        case COFFEE:
            return 500;

        case ORANGE_JUICE:
            return 300;

        case COCA_COLA:
        case PEPSI_COLA:
            return 200;

        case WATER:
            return 150;

        default:
            return 0;
    }
}

bool Good::isOfSameType(const Good &other) const {
    return type == other.type;
}
