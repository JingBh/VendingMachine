#include "VendingMachine.h"

void VendingMachine::init() {
    inventory.clear();
    inventory.push_back(std::make_unique<Good>(GoodType::COCA_COLA, 10));
    inventory.push_back(std::make_unique<Good>(GoodType::PEPSI_COLA, 10));
    inventory.push_back(std::make_unique<Good>(GoodType::ORANGE_JUICE, 10));
    inventory.push_back(std::make_unique<Good>(GoodType::COFFEE, 10));
    inventory.push_back(std::make_unique<Good>(GoodType::WATER, 10));

    cashBox.clear();
    cashBox.push_back(std::make_unique<Cash>(CashType::ONE_YUAN, 10));
    cashBox.push_back(std::make_unique<Cash>(CashType::FIFTY_CENTS, 10));
}
