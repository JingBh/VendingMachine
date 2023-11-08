#include "VendingMachine.h"

#include <algorithm>
#include <array>

#include "error/NotSufficientError.h"

const std::vector<std::unique_ptr<Good>> &VendingMachine::getInventory() const {
    return inventory;
}

void VendingMachine::putMoney(const std::vector<Cash> &payment) {
    for (auto &cash : payment) {
        bool flag = false;
        for (auto &item : cashBox) {
            if (item->getFaceValue() == cash.getFaceValue()) {
                item->fill(cash.getQuantity());
                flag = true;
                break;
            }
        }

        if (!flag) {
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

    userBalance -= good.getPrice() * good.getQuantity();
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

Money VendingMachine::getUserBalance() const {
    return userBalance;
}

void VendingMachine::refill() {
    std::array<GoodType, 5> goodTypes = {
        GoodType::COCA_COLA,
        GoodType::PEPSI_COLA,
        GoodType::ORANGE_JUICE,
        GoodType::COFFEE,
        GoodType::WATER
    };

    for (auto &goodType : goodTypes) {
        bool flag = false;
        for (auto &item : inventory) {
            if (item->getType() == goodType) {
                if (item->getQuantity() < 10) {
                    item->fill(10 - item->getQuantity());
                }

                flag = true;
                break;
            }
        }

        if (!flag) {
            inventory.emplace_back(std::make_unique<Good>(goodType, 10));
        }
    }

    std::array<CashType, 2> cashTypes = {
        CashType::ONE_YUAN,
        CashType::FIFTY_CENTS
    };

    for (auto &cashType : cashTypes) {
        bool flag = false;
        for (auto &item : cashBox) {
            if (item->getFaceValue() == Cash::valueOf(cashType)) {
                if (item->getQuantity() < 10) {
                    item->fill(10 - item->getQuantity());
                }

                flag = true;
                break;
            }
        }

        if (!flag) {
            cashBox.emplace_back(std::make_unique<Cash>(cashType, 10));
        }
    }
}
