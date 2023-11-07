#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

#ifndef VENDINGMACHINE_UI_UTIL_H
#define VENDINGMACHINE_UI_UTIL_H

void ui_enable_ansi();

std::string ansi_clear_screen();

std::string ansi_move_up(unsigned int lines);

#endif //VENDINGMACHINE_UI_UTIL_H
