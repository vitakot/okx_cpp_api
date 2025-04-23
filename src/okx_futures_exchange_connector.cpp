/**
OKX Exchange Connector

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2022 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include <vk/okx/okx_futures_exchange_connector.h>
#include "vk/okx/okx_futures_rest_client.h"
#include "vk/utils/semaphore.h"

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

namespace vk {
struct OKXFuturesExchangeConnector::P {
    std::shared_ptr<okx::futures::RESTClient> m_restClient{};
    std::vector<FundingRate> m_fundingRates;
    std::thread m_workerThread;
    std::atomic_bool m_isRunning{false};

    [[nodiscard]] std::vector<FundingRate> getFundingRates() const {
        const auto instruments = m_restClient->getInstruments(okx::InstrumentType::SWAP);

        std::vector<std::future<FundingRate>> futures;
        std::vector<FundingRate> readyFutures;

        constexpr int numJobs = 3;
        Semaphore m_maxConcurrentJobs{numJobs};
        int requestsDone = 0;

        auto t1 = high_resolution_clock::now();

        for (const auto& instrument : instruments) {
            spdlog::info("Getting FR for: {}...", instrument.m_instId);

            futures.push_back(
                std::async(std::launch::async,
                           [this, &requestsDone, t1
                           ](const std::string& instId, Semaphore& maxJobs) -> FundingRate {
                               std::scoped_lock w(maxJobs);

                               /// https://www.okx.com/docs-v5/en/#public-data-rest-api-get-funding-rate
                               constexpr double minMsPerRequest = numJobs * (2.0 / 20.0 * 1000.0) * 1.15;

                               const auto t1Fr = high_resolution_clock::now();
                               const auto fr = m_restClient->getLastFundingRate(instId);
                               const auto t2Fr = high_resolution_clock::now();

                               if (const duration<double, std::milli> msFr = t2Fr - t1Fr; msFr.count() <
                                   minMsPerRequest) {
                                   spdlog::info("Adding sleep: {} ms ",
                                                static_cast<int>(minMsPerRequest - msFr.count()));
                                   std::this_thread::sleep_for(
                                       milliseconds(static_cast<int>(minMsPerRequest - msFr.count())));
                               }

                               FundingRate fundingRate = {
                                   fr.m_instId, fr.m_fundingRate.convert_to<double>(), fr.m_nextFundingTime
                               };

                               requestsDone++;
                               const auto t2 = high_resolution_clock::now();
                               const duration<double, std::milli> ms = t2 - t1;

                               const auto speed = requestsDone / ms.count();
                               spdlog::info("Speed: {} requests per second", speed * 1000.0);

                               return fundingRate;
                           }, instrument.m_instId, std::ref(m_maxConcurrentJobs)));
        }

        do {
            for (auto& future : futures) {
                if (isReady(future)) {
                    readyFutures.push_back(future.get());
                    spdlog::info("Got FR for: {}, value : {}", readyFutures.back().symbol,
                                 readyFutures.back().fundingRate);
                }
            }
        }
        while (readyFutures.size() < futures.size());

        return readyFutures;
    }
};

OKXFuturesExchangeConnector::OKXFuturesExchangeConnector() : m_p(std::make_unique<P>()) {
}

OKXFuturesExchangeConnector::~OKXFuturesExchangeConnector() {
    if (m_p->m_workerThread.joinable()) {
        m_p->m_workerThread.join();
    }

    m_p->m_restClient.reset();
}

std::string OKXFuturesExchangeConnector::exchangeId() const {
    return std::string(magic_enum::enum_name(ExchangeId::MEXCFutures));
}

std::string OKXFuturesExchangeConnector::version() const {
    return "1.0.5";
}

void OKXFuturesExchangeConnector::setLoggerCallback(const onLogMessage& onLogMessageCB) {
}

void OKXFuturesExchangeConnector::login(const std::tuple<std::string, std::string, std::string>& credentials) {
    m_p->m_restClient.reset();
    m_p->m_restClient = std::make_shared<okx::futures::RESTClient>(std::get<0>(credentials),
                                                                   std::get<1>(credentials),
                                                                   std::get<2>(credentials));
}

Trade OKXFuturesExchangeConnector::placeOrder(const Order& order) {
    Trade retVal;
    throw std::runtime_error("Unimplemented: OKXFuturesExchangeConnector::placeOrder");
}

TickerPrice OKXFuturesExchangeConnector::getTickerPrice(const std::string& symbol) const {
    TickerPrice retVal;
    throw std::runtime_error("Unimplemented: OKXFuturesExchangeConnector::getTickerPrice");
}

Balance OKXFuturesExchangeConnector::getAccountBalance(const std::string& currency) const {
    Balance retVal;
    throw std::runtime_error("Unimplemented: OKXFuturesExchangeConnector::getAccountBalance");
}

FundingRate OKXFuturesExchangeConnector::getFundingRate(const std::string& symbol) const {
    const auto fr = m_p->m_restClient->getLastFundingRate(symbol);
    return {fr.m_instId, fr.m_fundingRate.convert_to<double>(), fr.m_nextFundingTime};
}

std::vector<FundingRate> OKXFuturesExchangeConnector::getFundingRates() const {
    if (m_p->m_isRunning) {
        return m_p->m_fundingRates;
    }

    m_p->m_isRunning = true;

    if (m_p->m_workerThread.joinable()) {
        m_p->m_workerThread.join();
    }

    m_p->m_workerThread = std::thread([&]() {
        m_p->m_fundingRates = m_p->getFundingRates();
        m_p->m_isRunning = false;
    });

    return m_p->m_fundingRates;
}

std::vector<Ticker> OKXFuturesExchangeConnector::getTickerInfo(const std::string& symbol) const {
    throw std::runtime_error("Unimplemented: OKXFuturesExchangeConnector::getTickerInfo");
}

std::int64_t OKXFuturesExchangeConnector::getServerTime() const {
    return m_p->m_restClient->getSystemTime();
}
}
