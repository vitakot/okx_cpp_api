#include "vk/okx/okx_futures_rest_client.h"
#include "vk/utils/json_utils.h"
#include "vk/utils/log_utils.h"
#include "vk/okx/okx_ws_stream_manager.h"
#include <spdlog/spdlog.h>
#include <filesystem>
#include <iostream>
#include <fstream>

using namespace vk::okx;
using namespace std::chrono_literals;

constexpr int HISTORY_LENGTH_IN_S = 86400; // 1 day

void logFunction(const vk::LogSeverity severity, const std::string &errmsg) {
    switch (severity) {
        case vk::LogSeverity::Info:
            spdlog::info(errmsg);
            break;
        case vk::LogSeverity::Warning:
            spdlog::warn(errmsg);
            break;
        case vk::LogSeverity::Critical:
            spdlog::critical(errmsg);
            break;
        case vk::LogSeverity::Error:
            spdlog::error(errmsg);
            break;
        case vk::LogSeverity::Debug:
            spdlog::debug(errmsg);
            break;
        case vk::LogSeverity::Trace:
            spdlog::trace(errmsg);
            break;
    }
}

void readCredentials(std::string &apiKey, std::string &apiSecret, std::string &passPhrase) {
    std::filesystem::path pathToCfg{"PATH_TO_CONFIG_FILE"};
    std::ifstream ifs(pathToCfg.string());

    if (!ifs.is_open()) {
        std::cerr << "Couldn't open config file: " + pathToCfg.string();
    }

    try {
        nlohmann::json json = nlohmann::json::parse(ifs);
        vk::readValue<std::string>(json, "ApiKey", apiKey);
        vk::readValue<std::string>(json, "ApiSecret", apiSecret);
        vk::readValue<std::string>(json, "PassPhrase", passPhrase);
    } catch (const std::exception &e) {
        std::cerr << e.what();
        ifs.close();
    }
}

void testData() {
    try {
        std::string apiSecret;
        std::string passPhrase;
        std::string apiKey;
        readCredentials(apiKey, apiSecret, passPhrase);
        const auto restClient = std::make_shared<futures::RESTClient>(apiKey, apiSecret, passPhrase);
        const auto nowTimestamp = std::chrono::seconds(std::time(nullptr)).count() * 1000;
        const auto oldestDate = (std::chrono::seconds(std::time(nullptr)).count() - 60 * 200) * 1000;
        auto candles = restClient->getHistoricalPrices("ETH-USDT-SWAP", BarSize::_1m, oldestDate, nowTimestamp);
    } catch (std::exception &e) {
        logFunction(vk::LogSeverity::Warning, fmt::format("Exception: {}", e.what()));
    }
}

[[noreturn]] void measureRestResponses() {
    std::string apiKey;
    std::string apiSecret;
    std::string passPhrase;

    readCredentials(apiKey, apiSecret, passPhrase);
    auto restClient = std::make_shared<futures::RESTClient>(apiKey, apiSecret, passPhrase);

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    double overallTime = 0.0;
    int numPass = 0;

    while (true) {
        auto t1 = high_resolution_clock::now();
        auto pr = restClient->getInstruments(InstrumentType::SWAP);
        auto t2 = high_resolution_clock::now();

        duration<double, std::milli> ms_double = t2 - t1;
        logFunction(vk::LogSeverity::Info, fmt::format("Get Instruments request time: {} ms", ms_double.count()));
        overallTime += ms_double.count();

        t1 = high_resolution_clock::now();
        auto ex = restClient->getLastFundingRate("ETH-USDT-SWAP");
        t2 = high_resolution_clock::now();

        ms_double = t2 - t1;
        logFunction(vk::LogSeverity::Info,
                    fmt::format("Get Last Funding Rate request time: {} ms", ms_double.count()));
        overallTime += ms_double.count();

        auto nowTimestamp = std::chrono::seconds(std::time(nullptr)).count() * 1000;
        auto oldestDate = (std::chrono::seconds(std::time(nullptr)).count() - 60 * 90) * 1000;
        t1 = high_resolution_clock::now();
        const auto account = restClient->getHistoricalPrices("ETH-USDT-SWAP", BarSize::_1m, oldestDate, nowTimestamp);
        t2 = high_resolution_clock::now();

        ms_double = t2 - t1;
        logFunction(vk::LogSeverity::Info, fmt::format("Get Historical Prices: {} ms\n", ms_double.count()));
        overallTime += ms_double.count();
        numPass++;

        double timePerResponse = overallTime / (numPass * 3);
        logFunction(vk::LogSeverity::Info, fmt::format("Average time per response: {} ms\n", timePerResponse));

        std::this_thread::sleep_for(2s);
    }
}

[[noreturn]] void testWebsockets() {
    const std::shared_ptr wsManager = std::make_unique<futures::WSStreamManager>();
    wsManager->setLoggerCallback(&logFunction);

    wsManager->subscribeTickersStream("ADA-USDT");

    while (true) {
        {
            if (const auto ret = wsManager->readEventInstrumentInfo("ADA-USDT")) {
                std::cout << fmt::format("ADA ask price: {}, bid price: {}", ret->m_tickers[0].m_askPx.str(),
                                         ret->m_tickers[0].m_bidPx.str())
                        << std::endl;
            } else {
                std::cout << "Error" << std::endl;
            }
        }
        std::this_thread::sleep_for(1000ms);
    }
}

void testBalance() {
    try {
        std::string passPhrase;
        std::string apiSecret;
        std::string apiKey;
        readCredentials(apiKey, apiSecret, passPhrase);
        const auto restClient = std::make_shared<futures::RESTClient>(apiKey, apiSecret, passPhrase);
        auto balance = restClient->getBalance("");
    } catch (std::exception &e) {
        logFunction(vk::LogSeverity::Warning, fmt::format("Exception: {}", e.what()));
    }
}

void testInstruments() {
    try {
        std::string apiSecret;
        std::string passPhrase;
        std::string apiKey;
        readCredentials(apiKey, apiSecret, passPhrase);
        const auto restClient = std::make_shared<futures::RESTClient>(apiKey, apiSecret, passPhrase);
        auto instruments = restClient->getInstruments(InstrumentType::MARGIN);
    } catch (std::exception &e) {
        logFunction(vk::LogSeverity::Warning, fmt::format("Exception: {}", e.what()));
    }
}

void testPositions() {
    try {
        std::string apiSecret;
        std::string passPhrase;
        std::string apiKey;
        readCredentials(apiKey, apiSecret, passPhrase);
        const auto restClient = std::make_shared<futures::RESTClient>(apiKey, apiSecret, passPhrase);
        auto positions = restClient->getPositions(InstrumentType::MARGIN, "ADA-USDT");
    } catch (std::exception &e) {
        logFunction(vk::LogSeverity::Warning, fmt::format("Exception: {}", e.what()));
    }
}

void testOrders() {
    try {
        std::string apiSecret;
        std::string passPhrase;
        std::string apiKey;
        readCredentials(apiKey, apiSecret, passPhrase);
        const auto restClient = std::make_shared<futures::RESTClient>(apiKey, apiSecret, passPhrase);

        Order order;
        order.m_instId = "ADA-USDT";
        order.m_side = Side::buy;
        order.m_ordType = OrderType::limit;
        order.m_sz = 10;
        order.m_px = 0.362;
        order.m_tdMode = MarginMode::cross;
        order.m_ccy = "USDT";

        auto orderResponses = restClient->placeOrder(order);
    } catch (std::exception &e) {
        logFunction(vk::LogSeverity::Warning, fmt::format("Exception: {}", e.what()));
    }
}

int main() {
    testOrders();
    return getchar();
}
