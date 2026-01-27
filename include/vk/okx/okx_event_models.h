/**
OKX Event Data Models

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2025 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#ifndef INCLUDE_VK_OKX_EVENT_MODELS_H
#define INCLUDE_VK_OKX_EVENT_MODELS_H

#include "vk/interface/i_json.h"
#include "vk/okx/okx_models.h"
#include <nlohmann/json.hpp>

namespace vk::okx {
struct WSSubscription final : IJson {
    std::string channel{};
    std::string instId{};

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct WSRequest final : IJson {
    OperationType op{OperationType::subscribe};
    std::vector<WSSubscription> subscriptions{};

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct WSResponse final : IJson {
    EventType event{EventType::subscribe};
    WSSubscription subscription{};
    std::string code{};
    std::string msg{};

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct DataEvent final : IJson {
    std::string channel{};
    std::string instId{};
    nlohmann::json data{};

    ~DataEvent() override = default;

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct DataEventCandlestick final : IJson {
    std::vector<Candle> candles{};

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct DataEventTicker final : IJson {
    std::vector<Ticker> tickers{};

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};
}
#endif //INCLUDE_VK_OKX_EVENT_MODELS_H
