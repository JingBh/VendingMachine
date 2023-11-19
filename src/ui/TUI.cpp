#include "TUI.h"

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>

#include "../core/error/NotSufficientError.h"
#include "../core/error/ValueError.h"
#include "../core/Cash.h"
#include "../core/Good.h"
#include "WebUI.h"

#ifdef _WIN32
#include <windows.h>
#endif

std::ostream &operator<<(std::ostream &os, const Money &money) {
    const long long int yuan = money.getValue() / 100;
    const long long int fen = money.getValue() % 100;

    os << "￥" << yuan << '.'
        << std::setw(2) << std::setfill('0') << fen
        << std::setw(0) << std::setfill(' ');

    return os;
}

TUI::TUI(const std::shared_ptr<VendingMachine> &machine) : machine(machine) {
#ifdef _WIN32
    // Enable UTF-8 support
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    // Enable ANSI support
    // Only works for Windows 10+
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif
}

void TUI::pageInit() const {
    ansiClearScreen();

    std::cout << ansiClearScreen()
        << "自动售卖机模拟系统" << '\n'
        << "by 22080206 敬博浩" << '\n'
        << std::endl;

    while (true) {
        int op;
        try {
            op = promptSelection("请选择界面类型：", {
                { 1, "命令行" },
                { 2, "Web" }
            });
        } catch (ValueError &) {
            std::cout << '\n'
                << "操作无效！"
                << "按 <Enter> 键继续..." << std::flush;
            waitForEnter();

            continue;
        }

        switch (op) {
            case 1:
                pageHome();
                break;
            case 2:
                pageWebUI();
                break;
            default:
                break;
        }

        break;
    }
}

void TUI::pageWebUI() const {
    std::cout << ansiClearScreen()
        << "# Web UI" << '\n'
        << std::endl;

    WebUI webUI(machine);
    webUI.start();
}

void TUI::pageHome() const {
    while (true) {
        std::cout << ansiClearScreen()
            << "欢迎使用自动售卖机！" << '\n'
            << std::endl;

        printStatus();
        std::cout << std::endl;

        int op;
        try {
            op = promptSelection({
                { 1, "投币" },
                { 2, "购买商品" },
                { 3, "补货（仅限工作人员）" },
                { 4, "统计信息（仅限工作人员）" },
                { 5, "退出" }
            });
        } catch (ValueError &) {
            std::cout << '\n'
                << "操作无效！"
                << "按 <Enter> 键继续..." << std::flush;
            waitForEnter();

            continue;
        }

        switch (op) {
            case 1:
                pageInsertCash();
                break;
            case 2:
                pagePurchase();
                break;
            case 3:
                pageRefill();
                break;
            case 4:
                pageStatistics();
                break;
            case 5:
                pageExit();
                return;
            default:
                break;
        }
    }
}

void TUI::pageInsertCash() const {
    std::cout << ansiClearScreen()
        << "# 投币" << '\n'
        << std::endl;

    int op;
    try {
        op = promptSelection("请选择要投入的现金类型：", {
            { TEN_YUAN, "十元" },
            { FIVE_YUAN, "五元" },
            { TWO_YUAN, "二元" },
            { ONE_YUAN, "一元" },
            { FIFTY_CENTS, "五角" }
        });
    } catch (ValueError &) {
        std::cout << '\n'
            << "操作无效！"
            << "按 <Enter> 键继续..." << std::flush;
        waitForEnter();

        return;
    }

    machine->userPutMoney({ Cash(static_cast<CashType>(op)) });
}

void TUI::pagePurchase() const {
    std::cout << ansiClearScreen()
        << "# 购买商品" << '\n'
        << std::endl;

    printStatus();
    std::cout << std::endl;

    int op;
    try {
        std::map<int, std::string> options;
        for (const auto &good : machine->getInventory()) {
            options[good->getType()] = good->getName();
        }

        if (options.empty()) {
            throw OutOfStockError("No goods available");
        }

        op = promptSelection("请选择要购买的商品：", options);
    } catch (ValueError &) {
        std::cout << '\n'
            << "操作无效！"
            << "按 <Enter> 键继续..." << std::flush;
        waitForEnter();

        return;
    } catch (OutOfStockError &) {
        std::cout << '\n'
            << "很抱歉，所有商品均已售罄！" << '\n'
            << "按 <Enter> 键继续..." << std::flush;
        waitForEnter();

        return;
    }

    try {
        const Good good(static_cast<GoodType>(op));
        machine->userPurchase(good);

        std::cout << '\n'
            << "购买成功！" << '\n'
            << "您获得了：" << good.getName() << " x" << good.getQuantity() << '\n'
            << "按 <Enter> 键继续..." << std::flush;
        waitForEnter();
    } catch (OutOfStockError &) {
        std::cout << '\n'
            << "很抱歉，商品已售罄！请选择其他商品或联系工作人员补货。" << '\n'
            << "按 <Enter> 键继续..." << std::flush;
        waitForEnter();
    } catch (MoneyNotSufficientError &) {
        std::cout << '\n'
            << "余额不足！请投入足够的现金后再试。" << '\n'
            << "按 <Enter> 键继续..." << std::flush;
        waitForEnter();
    }
}

void TUI::pageRefill() const {
    std::cout << ansiClearScreen()
        << "# 补货" << '\n'
        << std::endl;

    if (!machine->getCashBox().empty()) {
        std::cout << "零钱箱内容：" << '\n';
        for (const auto &cash : machine->getCashBox()) {
            std::cout << " - " << cash->getFaceValue() << '\t'
                      << cash->getQuantity() << " 张" << '\n';
        }
        std::cout << std::endl;
    }

    std::cout << "此操作会将每种商品余量补至 10 件" << '\n'
        << "并在零钱箱中补充 10 张一元现金和 10 张五角现金，用于找零" << '\n'
        << "确定要继续吗？" << '\n'
        << std::endl;

    try {
        const int op = promptSelection({
            { 1, "继续" },
            { 2, "取消" }
        });

        if (op == 1) {
            machine->refill();
            return;
        }
    } catch (ValueError &) {
        // pass
    }

    std::cout << '\n'
        << "操作已取消" << '\n'
        << "按 <Enter> 键继续..." << std::flush;
    waitForEnter();
}

void TUI::pageStatistics() const {
    std::cout << ansiClearScreen()
        << "# 统计信息" << '\n'
        << std::endl;

    Money sum = 0;
    if (!machine->getPurchaseHistory().empty()) {
        std::cout << "已售商品：" << '\n';
        for (const auto &good : machine->getPurchaseHistory()) {
            const Money value = good.getPrice().operator*(good.getQuantity());
            sum += value;

            std::cout << " - "
                << std::left << std::setfill(' ') << std::setw(12) << good.getName() << '\t'
                << std::left << std::setfill(' ') << std::setw(4) << "x" + std::to_string(good.getQuantity()) << '\t'
                << "销售额：" << value << '\n';
        }
        std::cout << '\n';
    }

    std::cout << "总销售额：" << sum << '\n'
        << std::endl;

    try {
        int op = promptSelection({
            { 1, "返回" },
            { 2, "清空数据" }
        });

        if (op == 2) {
            std::cout << '\n'
                << "确定要清空数据吗？清除后将不可恢复。" << '\n'
                << std::endl;

            op = promptSelection({
                { 1, "继续" },
                { 2, "取消" }
            });

            if (op == 1) {
                machine->clearPurchaseHistory();
            }
        }
    } catch (ValueError &) {
        // pass
    }
}

void TUI::pageExit() const {
    std::cout << ansiClearScreen();

    try {
        const auto changes = machine->userMakeChanges();

        if (!changes.empty()) {
            std::cout << "您获得了以下找零：" << '\n';
            for (const auto &cash : changes) {
                std::cout << " - " << cash.getFaceValue() << '\t'
                    << cash.getQuantity() << " 张" << '\n';
            }
            std::cout << '\n';
        }

        std::cout << "感谢您使用自动售卖机，欢迎下次光临！" << std::endl;
    } catch (MoneyNotSufficientError &) {
        std::cout << "由于售卖机内零钱不足，找零失败！" << '\n'
            << "请联系工作人员处理。" << std::endl;
    }
}

void TUI::printStatus() const {
    std::cout << "剩余商品：" << '\n';
    for (const auto &good : machine->getInventory()) {
        std::cout << " - "
            << std::left << std::setfill(' ') << std::setw(12) << good->getName() << '\t'
            << std::left << std::setfill(' ') << std::setw(4) << "x" + std::to_string(good->getQuantity()) << '\t'
            << "单价：" << good->getPrice() << '\n';
    }
    if (machine->getInventory().empty()) {
        std::cout << " - 售卖机空空如也，请联系工作人员补货。" << '\n';
    }
    std::cout << std::endl;

    std::cout << "已投金额：" << machine->getUserBalance() << std::endl;
}

int TUI::promptSelection(const std::map<int, std::string> &options) {
    return promptSelection("请选择操作：", options);
}

int TUI::promptSelection(const std::string &prompt, const std::map<int, std::string> &options) {
    if (options.begin() == options.end()) {
        return 0;
    }

    int offset = 0;
    if (options.begin()->first <= 0) {
        offset = -options.begin()->first + 1;
    }

    for (const auto& [key, value] : options) {
        std::cout << key + offset << ". " << value << '\n';
    }
    std::cout << prompt << std::flush;

    int op;
    std::cin >> op;
    op -= offset;

    if (std::cin.fail() || !options.contains(op)) {
        std::cin.clear();
        throw ValueError("Invalid option: " + std::to_string(op));
    }

    return op;
}

void TUI::waitForEnter() {
    std::cin.clear();
    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
}

std::string TUI::ansiClearScreen() {
#ifdef _WIN32
    std:system("cls");
#endif
    return "\033c";
}
