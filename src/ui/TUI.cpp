#include "TUI.h"

#include <iomanip>
#include <limits>

#include "../core/error/NotSufficientError.h"
#include "../core/error/ValueError.h"
#include "../core/Cash.h"
#include "../core/Good.h"

#ifdef _WIN32
#include <Windows.h>
#endif

std::ostream &operator<<(std::ostream &os, const Money &money) {
    long long int yuan = money.getValue() / 100;
    long long int fen = money.getValue() % 100;

    os << "￥" << yuan << '.'
        << std::setw(2) << std::setfill('0') << fen
        << std::setw(0) << std::setfill(' ');

    return os;
}

TUI::TUI(std::shared_ptr<VendingMachine> &machine) : machine(machine) {
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

void TUI::page_init() {
    ansi_clear_screen();

    std::cout << ansi_clear_screen()
        << "自动售卖机模拟系统" << '\n'
        << "by 22080206 敬博浩" << '\n'
        << std::endl;

    // TODO: select ui type
    page_home();
}

void TUI::page_home() {
    while (true) {
        std::cout << ansi_clear_screen()
            << "欢迎使用自动售卖机！" << '\n'
            << std::endl;

        print_status();
        std::cout << '\n';

        int op;
        try {
            op = prompt_selection({
                { 1, "投币" },
                { 2, "购买商品" },
                { 3, "补货（仅限工作人员）" },
                { 4, "退出" }
            });
        } catch (ValueError &e) {
            std::cout << "操作无效！按 <Enter> 键继续..." << std::flush;
            wait_for_enter();

            continue;
        }

        switch (op) {
            case 1:
                page_insert_cash();
                break;
            case 2:
                page_purchase();
                break;
            case 3:
                page_refill();
                break;
            case 4:
                page_exit();
                return;
            default:
                break;
        }
    }
}

void TUI::page_insert_cash() {
    std::cout << ansi_clear_screen()
        << "# 投币" << '\n'
        << std::endl;

    int op;
    try {
        op = prompt_selection("请选择要投入的现金类型：", {
            { CashType::TEN_YUAN, "十元" },
            { CashType::FIVE_YUAN, "五元" },
            { CashType::TWO_YUAN, "二元" },
            { CashType::ONE_YUAN, "一元" },
            { CashType::FIFTY_CENTS, "五角" }
        });
    } catch (ValueError &e) {
        std::cout << '\n'
            << "操作无效！"
            << "按 <Enter> 键继续..." << std::flush;
        wait_for_enter();

        return;
    }

    machine->putMoney({ Cash(static_cast<CashType>(op)) });
}

void TUI::page_purchase() {
    std::cout << ansi_clear_screen()
        << "# 购买商品" << '\n'
        << std::endl;

    print_status();
    std::cout << '\n';

    int op;
    try {
        std::map<int, std::string> options;
        for (const auto &good : machine->getInventory()) {
            options[good->getType()] = good->getName();
        }

        op = prompt_selection("请选择要购买的商品：", options);
    } catch (ValueError &e) {
        std::cout << '\n'
            << "操作无效！"
            << "按 <Enter> 键继续..." << std::flush;
        wait_for_enter();

        return;
    }

    try {
        Good good(static_cast<GoodType>(op));
        machine->purchase(good);

        std::cout << '\n'
            << "购买成功！" << '\n'
            << "您获得了：" << good.getName() << " x" << good.getQuantity() << '\n'
            << "按 <Enter> 键继续..." << std::flush;
        wait_for_enter();
    } catch (OutOfStockError &e) {
        std::cout << '\n'
            << "很抱歉，商品已售罄！请选择其他商品。" << '\n'
            << "按 <Enter> 键继续..." << std::flush;
        wait_for_enter();
    } catch (MoneyNotSufficientError &e) {
        std::cout << '\n'
            << "余额不足！请投入足够的现金后再试。" << '\n'
            << "按 <Enter> 键继续..." << std::flush;
        wait_for_enter();
    }
}

void TUI::page_refill() {
    std::cout << ansi_clear_screen()
        << "# 补货" << '\n'
        << '\n'
        << "此操作会将每种商品余量填充至 10 件" << '\n'
        << "并在零钱箱中填充 10 张一元现金和 10 张五角现金" << '\n'
        << "确定要继续吗？" << '\n'
        << std::endl;

    try {
        int op = prompt_selection({
            { 1, "继续" },
            { 2, "取消" }
        });

        if (op == 1) {
            machine->refill();
        }
    } catch (ValueError &e) {
        // pass
    }

    std::cout << '\n'
        << "操作已取消" << '\n'
        << "按 <Enter> 键继续..." << std::flush;
    wait_for_enter();
}

void TUI::page_exit() {
    std::cout << ansi_clear_screen();

    try {
        const auto changes = machine->makeChanges();

        if (!changes.empty()) {
            std::cout << "您获得了以下找零：" << '\n';
            for (const auto &cash : changes) {
                std::cout << " - " << cash.getFaceValue() << '\t'
                    << cash.getQuantity() << " 张" << '\n';
            }
            std::cout << '\n';
        }

        std::cout << "感谢您使用自动售卖机，欢迎下次光临！" << std::endl;
    } catch (MoneyNotSufficientError &e) {
        std::cout << "由于售卖机内零钱不足，找零失败！" << '\n'
            << "请联系工作人员处理。" << std::endl;
    }
}

void TUI::print_status() const {
    std::cout << "剩余商品：" << '\n';
    for (const auto &good : machine->getInventory()) {
        std::cout << " - "
            << std::left << std::setfill(' ') << std::setw(12) << good->getName() << '\t'
            << std::left << std::setfill(' ') << std::setw(4) << "x" + std::to_string(good->getQuantity()) << '\t'
            << "单价：" << good->getPrice() << '\n';
    }
    std::cout << std::endl;

    std::cout << "已投金额：" << machine->getUserBalance() << std::endl;
}

int TUI::prompt_selection(const std::map<int, std::string> &options) {
    return prompt_selection("请选择操作：", options);
}

int TUI::prompt_selection(const std::string &prompt, const std::map<int, std::string> &options) {
    if (options.begin() == options.end()) {
        return 0;
    }

    int offset = 0;
    if (options.begin()->first <= 0) {
        offset = -options.begin()->first + 1;
    }

    for (const auto& [key, value] : options) {
        std::cout << (key + offset) << ". " << value << '\n';
    }
    std::cout << prompt << std::flush;

    int op;
    std::cin >> op;
    op -= offset;

    if (std::cin.fail() || options.find(op) == options.end()) {
        std::cin.clear();
        throw ValueError("Invalid option: " + std::to_string(op));
    }

    return op;
}

void TUI::wait_for_enter() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string TUI::ansi_clear_screen() {
    return "\033c";
}
