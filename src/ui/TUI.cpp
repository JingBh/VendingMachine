#include "TUI.h"

#include <iomanip>
#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#endif

TUI::TUI(std::shared_ptr<VendingMachine> &machine) : machine(machine) {
#ifdef _WIN32
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

[[noreturn]] void TUI::page_home() {
    std::cout << ansi_clear_screen()
        << "欢迎使用自动售卖机！" << '\n'
        << std::endl;

    print_status();

    std::cout << '\n';
    while (true) {
        int op = prompt_selection({
            { 1, "投币" },
            { 2, "退出" }
        });

        switch (op) {
        case 1:
            break;
        case 2:
            break;
        default:
            std::cout << "无效操作！" << std::endl;
        }
    }
}

void TUI::print_status() const {
    // TODO: show goods and balance
}

int TUI::prompt_selection(const std::map<int, std::string> &options) {
    return prompt_selection("请选择操作：", options);
}

int TUI::prompt_selection(const std::string &prompt, const std::map<int, std::string> &options) {
    std::cout << '\n'; // placeholder
    for (const auto& [key, value] : options) {
        std::cout << key << ". " << value;
        if (key != options.rbegin()->first) {
            std::cout << '\n';
        }
    }
    std::cout << ansi_up_lines(options.size())
        << prompt << std::flush;

    int op;
    std::cin >> op;
    return op;
}

constexpr std::string TUI::ansi_clear_screen() {
    return "\033c";
}

std::string TUI::ansi_up_lines(uint8_t lines) {
    return "\033[" + std::to_string(lines) + "A\033[G";
}
