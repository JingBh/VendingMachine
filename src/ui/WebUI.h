#ifndef VENDINGMACHINE_WEBUI_H
#define VENDINGMACHINE_WEBUI_H

#include <memory>

#include <httplib.h>

#include "../core/VendingMachine.h"

class WebUI {
public:
    explicit WebUI(const std::shared_ptr<VendingMachine> &machine);

    void start();

    static constexpr int PORT = 8206;

private:
    std::shared_ptr<VendingMachine> machine;

    httplib::Server svr;

    void registerEndpoints();
};

#endif // VENDINGMACHINE_WEBUI_H
