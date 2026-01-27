/**
OKX WebSocket Stream manager

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2025 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#ifndef INCLUDE_VK_OKX_WS_STREAM_MANAGER_H
#define INCLUDE_VK_OKX_WS_STREAM_MANAGER_H

#include "vk/utils/log_utils.h"
#include "okx_event_models.h"
#include "okx_models.h"
#include <optional>

namespace vk::okx {
class WSStreamManager {
    struct P;
    std::unique_ptr<P> m_p{};

public:
    explicit WSStreamManager();

    ~ WSStreamManager();

    /**
     * Check if the Tickers Stream is subscribed for a selected pair, if not then subscribe it. When force parameter
     * is true then re-subscribe if already subscribed
     * @param instId instrument Id, e.g. "ETH-USDT-SWAP"
     */
    void subscribeTickersStream(const std::string &instId) const;

    /**
     * Check if the Candlestick Stream is subscribed for a selected instrument id, if not then subscribe it. When force parameter
     * is true then re-subscribe if already subscribed
     * @param instId instrument Id, e.g. "ETH-USDT-SWAP"
     * @param barSize
     */
    void subscribeCandlestickStream(const std::string &instId, BarSize barSize) const;

    /**
     * Set time of all reading operations
     * @param seconds
     */
    void setTimeout(int seconds) const;

    /**
     * Get time of all reading operations
     * @return
     */
    [[nodiscard]] int timeout() const;

    /**
     * Set logger callback, if no set then all errors are writen to the stderr stream only
     * @param onLogMessageCB
     */
    void setLoggerCallback(const onLogMessage &onLogMessageCB) const;

    /**
     * Try to read DataEventTicker structure. It will block at most Timeout time.
     * @param instId instrument Id, e.g. "ETH-USDT-SWAP"
     * @return EventInstrumentInfo structure if successful
     */
    [[nodiscard]] std::optional<DataEventTicker> readEventInstrumentInfo(const std::string &instId) const;

    /**
     * Try to read DataEventCandlestick structure. It will block at most Timeout time.
     * @param instId instrument Id, e.g. "ETH-USDT-SWAP"
     * @param barSize e.g BarSize::_1m
     * @return DataEventCandlestick structure if successful
     */
    [[nodiscard]] std::optional<DataEventCandlestick>
    readEventCandlestick(const std::string &instId, BarSize barSize) const;
};
}

#endif //INCLUDE_VK_OKX_WS_STREAM_MANAGER_H
