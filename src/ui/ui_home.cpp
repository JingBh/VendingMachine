#include "ui_home.h"

void ui_print_header() {
    ui_enable_ansi();
    ansi_clear_screen();

    std::cout << std::endl
        << "自动售卖机模拟系统" << std::endl
        << "by 22080206 敬博浩" << std::endl
        << std::endl;
}

void ui_print_home() {
    std::cout << "欢迎使用自动售卖机！" << std::endl;

    // TODO: 剩余商品 & 已投金额

    int op;
    std::cout << '\n' // placeholder
        << "1. 查看商品" << std::endl
        << "2. 投币" << ansi_move_up(2)
        << "请选择操作：" << std::flush;
    std::cin >> op;
}
