#include "VendingMachine.h"

#include <algorithm>

#include "error/NotSufficientError.h"

const std::vector<std::unique_ptr<Good>> &VendingMachine::getInventory() const {
    return inventory;
}

void VendingMachine::putMoney(const std::vector<Cash> &payment) {
    for (auto &cash : payment) {
        for (auto &item : cashBox) {
            if (item->getFaceValue() == cash.getFaceValue()) {
                item->fill(cash.getQuantity());
                break;
            }

            cashBox.emplace_back(std::make_unique<Cash>(cash.getFaceValue(), cash.getQuantity()));
        }

        userBalance += cash.getTotalValue();
    }
}

void VendingMachine::purchase(const Good &good) {
    if (userBalance < good.getPrice()) {
        throw MoneyNotSufficientError("Not enough money to purchase");
    }

    try {
        for (auto &item : inventory) {
            if (item->isOfSameType(good)) {
                item->draw(good.getQuantity());
                break;
            }
        }
    } catch (NotSufficientError &e) {
        throw OutOfStockError("Good out of stock");
    }
}

std::vector<Cash> VendingMachine::makeChanges() {
    std::sort(cashBox.begin(), cashBox.end(), [](const auto &a, const auto &b) {
        return a->getFaceValue() > b->getFaceValue();
    });

    std::vector<Cash> result;
    Money remainingAmount = userBalance;
    for (auto &cash : cashBox) {
        Money faceValue = cash->getFaceValue();
        unsigned int count = 0;

        while (remainingAmount >= faceValue && cash->getQuantity() > 0) {
            remainingAmount -= faceValue;
            cash->draw();
            count++;
        }

        if (count > 0) {
            result.emplace_back(faceValue, count);
        }
    }

    if (remainingAmount > 0) {
        // rollback operation
        for (auto &cash : result) {
            for (auto &item : cashBox) {
                if (item->getFaceValue() == cash.getFaceValue()) {
                    item->fill(cash.getQuantity());
                }
            }
        }

        throw MoneyNotSufficientError("Not enough money to make changes");
    }

    userBalance = 0;
    return result;
}

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
