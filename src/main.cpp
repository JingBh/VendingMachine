#include <memory>

#include "core/VendingMachine.h"
#include "ui/TUI.h"

int main() {
    // construct the vending machine
    std::shared_ptr<VendingMachine> machine = std::make_shared<VendingMachine>();

    // fill the vending machine
    machine->refill();

    // initialize UI
    TUI ui(machine);
    ui.page_init();

    return 0;
}
