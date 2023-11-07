#include "ui_util.h"

bool ansiEnabled = false;
void ui_enable_ansi() {
    if (ansiEnabled) {
        return;
    }

#ifdef _WIN32 // Only works for Windows 10+
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;

    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif

    ansiEnabled = true;
}

std::string ansi_clear_screen() {
    return "\033c";
}

std::string ansi_move_up(unsigned int lines) {
    return "\033[" + std::to_string(lines) + "A\033[G";
}
