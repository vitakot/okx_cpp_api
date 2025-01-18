/**
OKX Event Data Models

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2025 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include "vk/okx/okx_event_models.h"
#include "vk/utils/utils.h"
#include "vk/utils/json_utils.h"

namespace vk::okx::futures {
nlohmann::json WSSubscription::toJson() const {
    nlohmann::json json;
    json["channel"] = m_channel;
    json["instId"] = m_instId;
    return json;
}

void WSSubscription::fromJson(const nlohmann::json &json) {
    readValue<std::string>(json, "channel", m_channel);
    readValue<std::string>(json, "instId", m_instId);
}

nlohmann::json WSRequest::toJson() const {
    nlohmann::json json;
    json["op"] = m_op;

    auto args = nlohmann::json::array();

    for (const auto &subscription: m_subscriptions) {
        auto subJson = subscription.toJson();
        args.push_back(subJson);
    }

    json["args"] = args;
    return json;
}

void WSRequest::fromJson(const nlohmann::json &json) {
    throw std::runtime_error("Unimplemented: WSRequest::fromJson()");
}

nlohmann::json WSResponse::toJson() const {
    throw std::runtime_error("Unimplemented: WSResponse::toJson()");
}

void WSResponse::fromJson(const nlohmann::json &json) {
    readMagicEnum<EventType>(json, "event", m_event);

    if (m_event == EventType::error) {
        readValue<std::string>(json, "code", m_code);
        readValue<std::string>(json, "msg", m_msg);
    } else {
        const auto &arg = json["arg"];
        m_subscription.fromJson(arg);
    }
}

nlohmann::json DataEvent::toJson() const {
    throw std::runtime_error("Unimplemented: DataEvent::toJson()");
}

void DataEvent::fromJson(const nlohmann::json &json) {
    const auto &arg = json["arg"];
    readValue<std::string>(arg, "channel", m_channel);
    readValue<std::string>(arg, "instId", m_instId);
    m_data = json["data"];
}

nlohmann::json DataEventCandlestick::toJson() const {
    throw std::runtime_error("Unimplemented: DataEventCandlestick::toJson()");
}

void DataEventCandlestick::fromJson(const nlohmann::json &json) {
    for (const auto &el: json.items()) {
        Candle candle;
        candle.fromJson(el.value());
        m_candles.push_back(candle);
    }
}

nlohmann::json DataEventTicker::toJson() const {
    throw std::runtime_error("Unimplemented: DataEventCandlestick::toJson()");
}

void DataEventTicker::fromJson(const nlohmann::json &json) {
    for (const auto &el: json.items()) {
        Ticker ticker;
        ticker.fromJson(el.value());
        m_tickers.push_back(ticker);
    }
}
}
