/**
OKX Futures WebSocket Stream manager

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2025 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include "vk/okx/okx_futures_rest_client.h"
#include "vk/okx/okx_ws_stream_manager.h"
#include "vk/okx/okx_futures_ws_client.h"
#include "vk/okx/okx.h"
#include <mutex>
#include <thread>

#ifdef _WIN32
#include "magic_enum/magic_enum.hpp"
#else
#include "magic_enum.hpp"
#endif

using namespace std::chrono_literals;

namespace vk::okx::futures {
    struct WSStreamManager::P {
        std::unique_ptr<WebSocketClient> m_wsClient;
        int m_timeout = 5;
        mutable std::recursive_mutex m_tickersLocker;
        mutable std::recursive_mutex m_candlestickLocker;
        std::map<std::string, DataEventTicker> m_tickers;
        std::map<std::string, std::map<BarSize, DataEventCandlestick> > m_candlesticks;

        onLogMessage m_logMessageCB;

        explicit P() {
            m_wsClient = std::make_unique<WebSocketClient>();
            m_wsClient->setDataEventCallback([&](const DataEvent &event) {
                if (event.m_channel == "tickers") {
                    std::lock_guard lk(m_tickersLocker);

                    try {
                        DataEventTicker dataEventTicker;
                        dataEventTicker.fromJson(event.m_data);

                        if (const auto it = m_tickers.find(event.m_instId); it == m_tickers.end()) {
                            m_tickers.insert_or_assign(event.m_instId, dataEventTicker);
                        }
                    } catch (std::exception &e) {
                        m_logMessageCB(LogSeverity::Error, fmt::format("{}: {}", MAKE_FILELINE, e.what()));
                    }
                } else if (event.m_channel.find("candle") != std::string::npos) {
                    std::lock_guard lk(m_candlestickLocker);

                    try {
                        DataEventCandlestick eventCandlestick;
                        eventCandlestick.fromJson(event.m_data);

                        /// Insert new candle
                        {
                            auto it = m_candlesticks.find(event.m_instId);

                            if (it == m_candlesticks.end()) {
                                m_candlesticks.insert({event.m_instId, {}});
                            }

                            it = m_candlesticks.find(event.m_instId);
                            it->second.insert_or_assign(OKX::candlestickChannelToBarSize(
                                                            *magic_enum::enum_cast<CandlestickChannel>(
                                                                event.m_channel.c_str())), eventCandlestick);
                        }
                    } catch (std::exception &e) {
                        m_logMessageCB(LogSeverity::Error, fmt::format("{}: {}", MAKE_FILELINE, e.what()));
                    }
                }
            });
        }
    };

    WSStreamManager::WSStreamManager() : m_p(std::make_unique<P>()) {
    }

    WSStreamManager::~WSStreamManager() {
        m_p->m_wsClient.reset();
        m_p->m_timeout = 0;
    }

    void WSStreamManager::subscribeTickersStream(const std::string &instId) const {
        WSSubscription wsSubscription;
        wsSubscription.m_instId = instId;
        wsSubscription.m_channel = "tickers";

        if (std::string subscriptionRequest = wsSubscription.toJson().dump(); !m_p->m_wsClient->isSubscribed(
            subscriptionRequest)) {
            if (m_p->m_logMessageCB) {
                const auto msgString = fmt::format("subscribing: {}", subscriptionRequest);
                m_p->m_logMessageCB(LogSeverity::Info, msgString);
            }

            m_p->m_wsClient->subscribe(subscriptionRequest);
        }

        m_p->m_wsClient->run();
    }

    void WSStreamManager::subscribeCandlestickStream(const std::string &instId, const BarSize barSize) const {
        WSSubscription wsSubscription;
        wsSubscription.m_instId = instId;
        wsSubscription.m_channel = magic_enum::enum_name(OKX::barSizeToCandlestickChannel(barSize));

        if (std::string subscriptionRequest = wsSubscription.toJson().dump(); !m_p->m_wsClient->isSubscribed(
            subscriptionRequest)) {
            if (m_p->m_logMessageCB) {
                const auto msgString = fmt::format("subscribing: {}", subscriptionRequest);
                m_p->m_logMessageCB(LogSeverity::Info, msgString);
            }

            m_p->m_wsClient->subscribe(subscriptionRequest);
        }

        m_p->m_wsClient->run();
    }

    void WSStreamManager::setTimeout(const int seconds) const {
        m_p->m_timeout = seconds;
    }

    int WSStreamManager::timeout() const {
        return m_p->m_timeout;
    }

    void WSStreamManager::setLoggerCallback(const onLogMessage &onLogMessageCB) const {
        m_p->m_logMessageCB = onLogMessageCB;
        m_p->m_wsClient->setLoggerCallback(onLogMessageCB);
    }

    std::optional<DataEventTicker> WSStreamManager::readEventInstrumentInfo(const std::string &instId) const {
        int numTries = 0;
        const int maxNumTries = static_cast<int>(m_p->m_timeout / 0.01);

        while (numTries <= maxNumTries) {
            if (m_p->m_timeout == 0) {
                /// No need to wait when destroying object
                break;
            }

            m_p->m_tickersLocker.lock();

            if (const auto it = m_p->m_tickers.find(instId); it != m_p->m_tickers.end()) {
                auto retVal = it->second;
                m_p->m_tickersLocker.unlock();
                return retVal;
            }
            m_p->m_tickersLocker.unlock();
            numTries++;
            std::this_thread::sleep_for(3ms);
        }

        return {};
    }

    std::optional<DataEventCandlestick>
    WSStreamManager::readEventCandlestick(const std::string &instId, const BarSize barSize) const {
        int numTries = 0;
        const int maxNumTries = static_cast<int>(m_p->m_timeout / 0.01);

        while (numTries <= maxNumTries) {
            if (m_p->m_timeout == 0) {
                /// No need to wait when destroying object
                break;
            }

            m_p->m_candlestickLocker.lock();

            if (const auto it = m_p->m_candlesticks.find(instId); it != m_p->m_candlesticks.end()) {
                if (const auto itCandle = it->second.find(barSize); itCandle != it->second.end()) {
                    auto retVal = itCandle->second;
                    m_p->m_candlestickLocker.unlock();
                    return retVal;
                }
            }
            m_p->m_candlestickLocker.unlock();
            numTries++;
            std::this_thread::sleep_for(3ms);
        }
        return {};
    }
}
