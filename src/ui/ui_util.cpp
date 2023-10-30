#include "ui_util.h"

void ui_clear_screen() {
    ui_enable_ansi();
    std::cout << "\033c";
}

bool ansiEnabled = false;
void ui_enable_ansi() {
    if (ansiEnabled) {
        return;
    }

    // Only works for Windows 10+

    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;

    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);

    ansiEnabled = true;
}
