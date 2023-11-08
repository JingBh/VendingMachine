#include <memory>
#include <vector>

#include "Cash.h"
#include "Good.h"
#include "Money.h"

#ifndef VENDINGMACHINE_VENDINGMACHINE_H
#define VENDINGMACHINE_VENDINGMACHINE_H

class VendingMachine {
public:
    VendingMachine() {
        init();
    }

    const std::vector<std::unique_ptr<Good>> &getInventory() const;

    void putMoney(const std::vector<Cash> &payment);

    void purchase(const Good &good);

    std::vector<Cash> makeChanges();

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

    /**
     * Initialize the machine.
     *
     * On start, the machine has
     * a stock of 10 for each good,
     * 10 one yuan and 10 fifty cents cash.
     */
    void init();
};

#endif //VENDINGMACHINE_VENDINGMACHINE_H
