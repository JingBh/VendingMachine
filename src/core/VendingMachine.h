#ifndef VENDINGMACHINE_VENDINGMACHINE_H
#define VENDINGMACHINE_VENDINGMACHINE_H

#include <fstream>
#include <memory>
#include <vector>

#include <nlohmann/json.hpp>

#include "contract/HasPersistence.h"
#include "Cash.h"
#include "Good.h"
#include "Money.h"

class VendingMachine final : HasPersistence {
public:
    VendingMachine();

    virtual ~VendingMachine() = default;

    const std::vector<std::unique_ptr<Good>> &getInventory() const;

    const std::vector<std::unique_ptr<Cash>> &getCashBox() const;

    Money getUserBalance() const;

    void userPutMoney(const std::vector<Cash> &payment);

    void userPurchase(const Good &good);

    std::vector<Cash> userMakeChanges();

    void refill();

private:
    Money userBalance = 0;

    /**
     * Inventory for storing goods
     */
    std::vector<std::unique_ptr<Good>> inventory;

    /**
     * Cash box for charging cash and making changes
     */
    std::vector<std::unique_ptr<Cash>> cashBox;

    void putMoney(const Cash &cash, bool absolute = false);

    void putGood(const Good &good, bool absolute = false);

    void saveState(std::ofstream &os) const override;

    void loadState(std::ifstream &is) override;
};

void to_json(nlohmann::json &j, const VendingMachine &obj);

#endif //VENDINGMACHINE_VENDINGMACHINE_H
