#ifndef VENDINGMACHINE_TUI_H
#define VENDINGMACHINE_TUI_H

#include <iosfwd>
#include <map>
#include <memory>
#include <string>

#include "../core/Money.h"
#include "../core/VendingMachine.h"

std::ostream &operator<<(std::ostream &os, const Money &money);

class TUI {
public:
    explicit TUI(const std::shared_ptr<VendingMachine> &machine);

    void pageInit() const;

    void pageHome() const;

    void pageInsertCash() const;

    void pagePurchase() const;

    void pageRefill() const;

    void pageExit() const;

private:
    std::shared_ptr<VendingMachine> machine;

    void printStatus() const;

    static int promptSelection(const std::map<int, std::string> &options);

    static int promptSelection(const std::string &prompt, const std::map<int, std::string> &options);

    static void waitForEnter();

    static std::string ansiClearScreen();
};

#endif // VENDINGMACHINE_TUI_H
