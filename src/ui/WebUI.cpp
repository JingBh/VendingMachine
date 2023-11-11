#include "WebUI.h"

#include <cstdlib>
#include <iostream>
#include <string>

#include <nlohmann/json.hpp>

WebUI::WebUI(const std::shared_ptr<VendingMachine> &machine) : machine(machine) {
    registerEndpoints();
}

void WebUI::start() {
    if (!svr.bind_to_port("localhost", PORT)) {
        std::cerr << "服务器启动失败！" << '\n'
            << "请检查 " << PORT << " 端口是否已被占用。" << std::endl;
        return;
    }

    // open browser and navigate to the page
    std::string command;
#ifdef _WIN32
    command = "start";
#elif __APPLE__
    command = "open";
#elif __linux__
    command = "xdg-open";
#endif
    if (!command.empty()) {
        command += " http://localhost:" + std::to_string(PORT);
        std::system(command.c_str());
    }

    std::cout << "若浏览器没有自动打开，请手动访问 http://localhost:" << PORT << '\n'
        << "若出现网页白屏等现象，请用最新版本的浏览器重试（不要使用 IE）。" << std::endl;

    svr.listen_after_bind();
}

void WebUI::registerEndpoints() {
    using namespace httplib;

    svr.Get("/", [&](const Request &, Response &res) {
#ifndef VENDINGMACHINE_WEBUI_NOASSET
#include "index.html.h"
        res.set_content(reinterpret_cast<const char*>(index_html), index_html_len, "text/html; charset=utf-8");
#else
        res.set_content("未找到网页资源，请参考课设报告书中相关说明正确编译。", "text/plain; charset=utf-8");
#endif
    });

    svr.Get("/api/state", [&](const Request &, Response &res) {
        const nlohmann::json j = *machine;
        res.set_content(j.dump(), "application/json");
    });

    svr.Post("/api/stop", [&](const Request &, Response &) {
        svr.stop();
    });
}
