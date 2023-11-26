#include "WebUI.h"

#include <cstdlib>
#include <iostream>
#include <string>

#include <nlohmann/json.hpp>

#include "../core/error/NotSufficientError.h"
#include "../core/Good.h"

WebUI::WebUI(const std::shared_ptr<VendingMachine> &machine)
    : machine{machine} {
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
        res.set_content("未找到网页资源，请参考课设报告中相关说明重新编译，或选择其它类型的用户界面。", "text/plain; charset=utf-8");
#endif
    });

    svr.Get("/api/state", [&](const Request &, Response &res) {
        nlohmann::json json;

        const nlohmann::json jsonMachine = *machine;
        json.emplace("machine", jsonMachine);

        nlohmann::json jsonGoods;
        for (auto &goodType : {
            COCA_COLA,
            PEPSI_COLA,
            ORANGE_JUICE,
            COFFEE,
            WATER
        }) {
            jsonGoods.push_back({
                { "id", goodType },
                { "imageId", Good(goodType).getImageId() },
                { "name", Good(goodType).getName() },
                { "price", Good(goodType).getPrice() }
            });
        }
        json.emplace("goods", jsonGoods);

        const nlohmann::json jsonCashes = {
            {
                { "imageId", "ten-yuan" },
                { "name", "十元" },
                { "value", 1000 }
            },
            {
                { "imageId", "five-yuan" },
                { "name", "五元" },
                { "value", 500 }
            },
            {
                { "imageId", "two-yuan" },
                { "name", "二元" },
                { "value", 200 }
            },
            {
                { "imageId", "one-yuan" },
                { "name", "一元" },
                { "value", 100 }
            },
            {
                { "imageId", "fifty-cents" },
                { "name", "五角" },
                { "value", 50 }
            }
        };
        json.emplace("cashes", jsonCashes);

        res.set_content(json.dump(), "application/json; charset=utf-8");
    });

    svr.Post("/api/cash", [&](const Request &req, Response &res) {
        if (!req.has_param("value")) {
            res.status = 400;
            return;
        }

        const Cash cash(std::stoll(req.get_param_value("value")));
        machine->userPutMoney({ cash });

        res.status = 201;
    });

    svr.Post("/api/purchase", [&](const Request &req, Response &res) {
        if (!req.has_param("id")) {
            res.status = 400;
            return;
        }

        try {
            const Good good(static_cast<GoodType>(std::stoi(req.get_param_value("id"))));
            machine->userPurchase(good);
        } catch (MoneyNotSufficientError &) {
            res.status = 402;
            res.set_content("余额不足！请投入足够的现金后再试。", "text/plain; charset=utf-8");
            return;
        } catch (OutOfStockError &) {
            res.status = 402;
            res.set_content("很抱歉，商品已售罄！请选择其他商品或联系工作人员补货。", "text/plain; charset=utf-8");
            return;
        }

        res.status = 201;
    });

    svr.Post("/api/refill", [&](const Request &, Response &res) {
        machine->refill();

        res.status = 201;
    });

    svr.Post("/api/history/clear", [&](const Request &, Response &res) {
        machine->clearPurchaseHistory();

        res.status = 201;
    });

    svr.Post("/api/exit", [&](const Request &, Response &res) {
        try {
            const auto changes = machine->userMakeChanges();

            const nlohmann::json j = changes;
            res.set_content(j.dump(), "application/json; charset=utf-8");
        } catch (MoneyNotSufficientError &) {
            res.status = 402;
            res.set_content("由于售卖机内零钱不足，找零失败！请联系工作人员处理。", "text/plain; charset=utf-8");
        }
    });

    svr.Post("/api/stop", [&](const Request &, Response &) {
        svr.stop();
    });
}
