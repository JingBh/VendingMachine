#include <memory>

#include "core/VendingMachine.h"
#include "ui/TUI.h"

int main() {
    // construct the vending machine
    std::shared_ptr<VendingMachine> machine = std::make_shared<VendingMachine>();

    TUI ui(machine);
    ui.page_init();

    return 0;
}
