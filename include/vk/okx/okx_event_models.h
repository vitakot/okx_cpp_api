/**
OKX Event Data Models

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2025 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#ifndef INCLUDE_VK_OKX_EVENT_MODELS_H
#define INCLUDE_VK_OKX_EVENT_MODELS_H

#include "vk/tools/i_json.h"
#include "vk/okx/okx_models.h"
#include <nlohmann/json.hpp>

namespace vk::okx::futures {
enum class OperationType : std::int32_t {
    subscribe,
    unsubscribe
};

enum class EventType : std::int32_t {
    subscribe,
    unsubscribe,
    error
};

enum class CandlestickChannel : std::int32_t {
    candle1m,
    candle3m,
    candle5m,
    candle15m,
    candle30m,
    candle1H,
    candle2H,
    candle4H,
    candle6H,
    candle12H,
    candle1D,
    candle2D,
    candle3D,
    candle1W,
    candle1M,
    candle3M
};

struct WSSubscription final : IJson {
    std::string m_channel{};
    std::string m_instId{};

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct WSRequest final : IJson {
    OperationType m_op{OperationType::subscribe};
    std::vector<WSSubscription> m_subscriptions{};

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct WSResponse final : IJson {
    EventType m_event{EventType::subscribe};
    WSSubscription m_subscription{};
    std::string m_code{};
    std::string m_msg{};

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct DataEvent final : IJson {
    std::string m_channel{};
    std::string m_instId{};
    nlohmann::json m_data{};

    ~DataEvent() override = default;

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct DataEventCandlestick final : IJson {
    std::vector<Candle> m_candles{};

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct DataEventTicker final : IJson {
    std::vector<Ticker> m_tickers{};

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};
}
#endif //INCLUDE_VK_OKX_EVENT_MODELS_H
