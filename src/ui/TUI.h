#include <map>
#include <memory>
#include <string>

#include "../core/VendingMachine.h"

class TUI {
public:
    explicit TUI(std::shared_ptr<VendingMachine> &machine);

    void page_init();

    [[noreturn]] void page_home();

private:
    std::shared_ptr<VendingMachine> machine;

    void print_status() const;

    static int prompt_selection(const std::map<int, std::string> &options);

    static int prompt_selection(const std::string &prompt, const std::map<int, std::string> &options);

    static constexpr std::string ansi_clear_screen();

    static std::string ansi_up_lines(uint8_t lines);
};
