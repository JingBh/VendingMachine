#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "../core/Money.h"
#include "../core/VendingMachine.h"

#ifndef VENDINGMACHINE_TUI_H
#define VENDINGMACHINE_TUI_H

std::ostream &operator<<(std::ostream &os, const Money &money);

class TUI {
public:
    explicit TUI(std::shared_ptr<VendingMachine> &machine);

    void page_init();

    void page_home();

    void page_insert_cash();

    void page_purchase();

    void page_refill();

    void page_exit();

private:
    std::shared_ptr<VendingMachine> machine;

    void print_status() const;

    static int prompt_selection(const std::map<int, std::string> &options);

    static int prompt_selection(const std::string &prompt, const std::map<int, std::string> &options);

    static void wait_for_enter();

    static std::string ansi_clear_screen();
};

#endif // VENDINGMACHINE_TUI_H
