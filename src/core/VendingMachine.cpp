#include "VendingMachine.h"

#include <algorithm>
#include <fstream>

#include "error/NotSufficientError.h"

VendingMachine::VendingMachine() : HasPersistence("machine") {
    HasPersistence::loadState();
}

VendingMachine::VendingMachine(const Money userBalance, const std::vector<Good> &inventory, const std::vector<Cash> &cashBox)
    : HasPersistence("machine"), userBalance(userBalance) {
    persistenceEnabled = false;

    for (const auto &good : inventory) {
        this->inventory.push_back(std::make_unique<Good>(good));
    }

    for (const auto &cash : cashBox) {
        this->cashBox.push_back(std::make_unique<Cash>(cash));
    }

    this->userBalance = userBalance;
}

const std::vector<std::unique_ptr<Good>> &VendingMachine::getInventory() const {
    return inventory;
}

const std::vector<std::unique_ptr<Cash>> &VendingMachine::getCashBox() const {
    return cashBox;
}

Money VendingMachine::getUserBalance() const {
    return userBalance;
}

void VendingMachine::userPutMoney(const std::vector<Cash> &payment) {
    for (auto &cash : payment) {
        putMoney(cash);
        userBalance += cash.getTotalValue();
    }

    saveState();
}

void VendingMachine::userPurchase(const Good &good) {
    if (userBalance < good.getPrice()) {
        throw MoneyNotSufficientError("Not enough money to purchase");
    }

    try {
        for (const auto &item : inventory) {
            if (item->isOfSameType(good)) {
                item->draw(good.getQuantity());
                break;
            }
        }
    } catch (NotSufficientError &) {
        throw OutOfStockError("Good out of stock");
    }

    userBalance -= good.getPrice().operator*(good.getQuantity());

    bool flag = false;
    for (auto &item : purchaseHistory) {
        if (item.isOfSameType(good)) {
            item.fill(good.getQuantity());
            flag = true;
            break;
        }
    }
    if (!flag) {
        purchaseHistory.emplace_back(good);
    }

    saveState();
}

std::vector<Cash> VendingMachine::userMakeChanges() {
    std::ranges::sort(cashBox, [](const auto &a, const auto &b) {
        return a->getFaceValue() > b->getFaceValue();
    });

    std::vector<Cash> result;
    Money remainingAmount = userBalance;
    for (const auto &cash : cashBox) {
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
            putMoney(cash);
        }

        throw MoneyNotSufficientError("Not enough money to make changes");
    }

    userBalance = 0;

    saveState();

    return result;
}

void VendingMachine::refill() {
    for (auto &goodType : {
        COCA_COLA,
        PEPSI_COLA,
        ORANGE_JUICE,
        COFFEE,
        WATER
    }) {
        putGood(Good(goodType, 10), true);
    }

    for (auto &cashType : {
        ONE_YUAN,
        FIFTY_CENTS
    }) {
        putMoney(Cash(cashType, 10), true);
    }

    saveState();
}

const std::vector<Good> &VendingMachine::getPurchaseHistory() const {
    return purchaseHistory;
}

void VendingMachine::clearPurchaseHistory() {
    purchaseHistory.clear();

    saveState();
}

void VendingMachine::putMoney(const Cash &cash, const bool absolute) {
    bool flag = false;
    for (const auto &item : cashBox) {
        if (item->getFaceValue() == cash.getFaceValue()) {
            if (absolute) {
                if (item->getQuantity() < cash.getQuantity()) {
                    item->fill(cash.getQuantity() - item->getQuantity());
                }
            } else {
                item->fill(cash.getQuantity());
            }

            flag = true;
            break;
        }
    }

    if (!flag) {
        cashBox.emplace_back(std::make_unique<Cash>(cash.getFaceValue(), cash.getQuantity()));
    }
}

void VendingMachine::putGood(const Good &good, const bool absolute) {
    bool flag = false;
    for (const auto &item : inventory) {
        if (item->isOfSameType(good)) {
            if (absolute) {
                if (item->getQuantity() < good.getQuantity()) {
                    item->fill(good.getQuantity() - item->getQuantity());
                }
            } else {
                item->fill(good.getQuantity());
            }

            flag = true;
            break;
        }
    }

    if (!flag) {
        inventory.emplace_back(std::make_unique<Good>(good.getType(), good.getQuantity()));
    }
}

void VendingMachine::saveState() const {
    if (persistenceEnabled) {
        HasPersistence::saveState();
    }
}

void VendingMachine::saveState(std::ofstream &os) const {
    const nlohmann::json j = *this;
    os << j;
}

void VendingMachine::loadState(std::ifstream &is) {
    nlohmann::json j;
    is >> j;

    inventory.clear();
    for (const auto &item : j.at("inventory")) {
        putGood(item, true);
    }

    cashBox.clear();
    for (const auto &item : j.at("cashBox")) {
        putMoney(item, true);
    }

    purchaseHistory.clear();
    for (const auto &item : j.at("purchaseHistory")) {
        purchaseHistory.push_back(item);
    }

    userBalance = j.at("userBalance");
}

void to_json(nlohmann::json &j, const VendingMachine &obj) {
    j = nlohmann::json{};

    nlohmann::json jsonInventory;
    for (const auto &item : obj.getInventory()) {
        jsonInventory.push_back(*item);
    }
    j.emplace("inventory", jsonInventory);

    nlohmann::json jsonCashBox;
    for (const auto &item : obj.getCashBox()) {
        jsonCashBox.push_back(*item);
    }
    j.emplace("cashBox", jsonCashBox);

    j.emplace("userBalance", obj.getUserBalance());

    nlohmann::json jsonPurchaseHistory;
    for (const auto &item : obj.getPurchaseHistory()) {
        jsonPurchaseHistory.push_back(item);
    }
    j.emplace("purchaseHistory", jsonPurchaseHistory);
}
