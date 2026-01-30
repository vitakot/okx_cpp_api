/**
OKX WebSocket Stream manager

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2025 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include "vk/okx/okx_rest_client.h"
#include "vk/okx/okx_ws_stream_manager.h"
#include "vk/okx/okx_ws_client.h"
#include "vk/okx/okx.h"
#include <mutex>
#include <thread>

#ifdef _WIN32
#include "magic_enum/magic_enum.hpp"
#else
#include "magic_enum/magic_enum.hpp"
#endif

using namespace std::chrono_literals;

namespace vk::okx {
struct WSStreamManager::P {
    std::unique_ptr<WebSocketClient> wsClient;
    int timeout = 5;
    mutable std::recursive_mutex tickersLocker;
    mutable std::recursive_mutex candlestickLocker;
    std::map<std::string, DataEventTicker> tickers;
    std::map<std::string, std::map<BarSize, DataEventCandlestick> > candlesticks;
    onLogMessage logMessageCB;

    explicit P() {
        wsClient = std::make_unique<WebSocketClient>();
        wsClient->setDataEventCallback([&](const DataEvent &event) {
            if (event.channel == "tickers") {
                std::lock_guard lk(tickersLocker);

                try {
                    DataEventTicker dataEventTicker;
                    dataEventTicker.fromJson(event.data);

                    if (const auto it = tickers.find(event.instId); it == tickers.end()) {
                        tickers.insert_or_assign(event.instId, dataEventTicker);
                    }
                } catch (std::exception &e) {
                    logMessageCB(LogSeverity::Error, fmt::format("{}: {}", MAKE_FILELINE, e.what()));
                }
            } else if (event.channel.find("candle") != std::string::npos) {
                std::lock_guard lk(candlestickLocker);

                try {
                    DataEventCandlestick eventCandlestick;
                    eventCandlestick.fromJson(event.data);

                    /// Insert new candle
                    {
                        auto it = candlesticks.find(event.instId);

                        if (it == candlesticks.end()) {
                            candlesticks.insert({event.instId, {}});
                        }

                        it = candlesticks.find(event.instId);
                        it->second.insert_or_assign(OKX::candlestickChannelToBarSize(
                                                        *magic_enum::enum_cast<CandlestickChannel>(
                                                            event.channel)), eventCandlestick);
                    }
                } catch (std::exception &e) {
                    logMessageCB(LogSeverity::Error, fmt::format("{}: {}", MAKE_FILELINE, e.what()));
                }
            }
        });
    }
};

WSStreamManager::WSStreamManager() : m_p(std::make_unique<P>()) {
}

WSStreamManager::~WSStreamManager() {
    m_p->wsClient.reset();
    m_p->timeout = 0;
}

void WSStreamManager::subscribeTickersStream(const std::string &instId) const {
    WSSubscription wsSubscription;
    wsSubscription.instId = instId;
    wsSubscription.channel = "tickers";

    if (std::string subscriptionRequest = wsSubscription.toJson().dump(); !m_p->wsClient->isSubscribed(
        subscriptionRequest)) {
        if (m_p->logMessageCB) {
            const auto msgString = fmt::format("subscribing: {}", subscriptionRequest);
            m_p->logMessageCB(LogSeverity::Info, msgString);
        }

        m_p->wsClient->subscribe(subscriptionRequest);
    }

    m_p->wsClient->run();
}

void WSStreamManager::subscribeCandlestickStream(const std::string &instId, const BarSize barSize) const {
    WSSubscription wsSubscription;
    wsSubscription.instId = instId;
    wsSubscription.channel = magic_enum::enum_name(OKX::barSizeToCandlestickChannel(barSize));

    if (std::string subscriptionRequest = wsSubscription.toJson().dump(); !m_p->wsClient->isSubscribed(
        subscriptionRequest)) {
        if (m_p->logMessageCB) {
            const auto msgString = fmt::format("subscribing: {}", subscriptionRequest);
            m_p->logMessageCB(LogSeverity::Info, msgString);
        }

        m_p->wsClient->subscribe(subscriptionRequest);
    }

    m_p->wsClient->run();
}

void WSStreamManager::setTimeout(const int seconds) const {
    m_p->timeout = seconds;
}

int WSStreamManager::timeout() const {
    return m_p->timeout;
}

void WSStreamManager::setLoggerCallback(const onLogMessage &onLogMessageCB) const {
    m_p->logMessageCB = onLogMessageCB;
    m_p->wsClient->setLoggerCallback(onLogMessageCB);
}

std::optional<DataEventTicker> WSStreamManager::readEventInstrumentInfo(const std::string &instId) const {
    int numTries = 0;
    const int maxNumTries = static_cast<int>(m_p->timeout / 0.01);

    while (numTries <= maxNumTries) {
        if (m_p->timeout == 0) {
            /// No need to wait when destroying object
            break;
        }

        m_p->tickersLocker.lock();

        if (const auto it = m_p->tickers.find(instId); it != m_p->tickers.end()) {
            auto retVal = it->second;
            m_p->tickersLocker.unlock();
            return retVal;
        }
        m_p->tickersLocker.unlock();
        numTries++;
        std::this_thread::sleep_for(3ms);
    }

    return {};
}

std::optional<DataEventCandlestick>
WSStreamManager::readEventCandlestick(const std::string &instId, const BarSize barSize) const {
    int numTries = 0;
    const int maxNumTries = static_cast<int>(m_p->timeout / 0.01);

    while (numTries <= maxNumTries) {
        if (m_p->timeout == 0) {
            /// No need to wait when destroying object
            break;
        }

        m_p->candlestickLocker.lock();

        if (const auto it = m_p->candlesticks.find(instId); it != m_p->candlesticks.end()) {
            if (const auto itCandle = it->second.find(barSize); itCandle != it->second.end()) {
                auto retVal = itCandle->second;
                m_p->candlestickLocker.unlock();
                return retVal;
            }
        }
        m_p->candlestickLocker.unlock();
        numTries++;
        std::this_thread::sleep_for(3ms);
    }
    return {};
}
}
