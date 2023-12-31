#include <memory>

#include "core/VendingMachine.h"
#include "ui/TUI.h"

int main() {
    // construct the vending machine
    const auto machine = std::make_shared<VendingMachine>();

    // initialize UI
    const TUI ui(machine);
    ui.pageInit();

    return 0;
}
