#ifndef VENDINGMACHINE_VENDINGMACHINE_H
#define VENDINGMACHINE_VENDINGMACHINE_H

#include <iosfwd>
#include <memory>
#include <vector>

#include <nlohmann/json.hpp>

#include "contract/HasPersistence.h"
#include "Cash.h"
#include "Good.h"
#include "Money.h"

class VendingMachine final : public HasPersistence {
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

    const std::vector<Good> &getPurchaseHistory() const;

    void clearPurchaseHistory();

private:
    Money userBalance = 0;

    std::vector<std::unique_ptr<Good>> inventory;

    std::vector<std::unique_ptr<Cash>> cashBox;

    std::vector<Good> purchaseHistory;

    void putMoney(const Cash &cash, bool absolute = false);

    void putGood(const Good &good, bool absolute = false);

    void saveState(std::ofstream &os) const override;

    void loadState(std::ifstream &is) override;
};

void to_json(nlohmann::json &j, const VendingMachine &obj);

#endif //VENDINGMACHINE_VENDINGMACHINE_H
