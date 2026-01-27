/**
OKX Event Data Models

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2025 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include "vk/okx/okx_event_models.h"
#include "vk/utils/utils.h"
#include "vk/utils/json_utils.h"

namespace vk::okx {
nlohmann::json WSSubscription::toJson() const {
    nlohmann::json json;
    json["channel"] = channel;
    json["instId"] = instId;
    return json;
}

void WSSubscription::fromJson(const nlohmann::json &json) {
    readValue<std::string>(json, "channel", channel);
    readValue<std::string>(json, "instId", instId);
}

nlohmann::json WSRequest::toJson() const {
    nlohmann::json json;
    json["op"] = op;

    auto args = nlohmann::json::array();

    for (const auto &subscription: subscriptions) {
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
    readMagicEnum<EventType>(json, "event", event);

    if (event == EventType::error) {
        readValue<std::string>(json, "code", code);
        readValue<std::string>(json, "msg", msg);
    } else {
        const auto &arg = json["arg"];
        subscription.fromJson(arg);
    }
}

nlohmann::json DataEvent::toJson() const {
    throw std::runtime_error("Unimplemented: DataEvent::toJson()");
}

void DataEvent::fromJson(const nlohmann::json &json) {
    const auto &arg = json["arg"];
    readValue<std::string>(arg, "channel", channel);
    readValue<std::string>(arg, "instId", instId);
    data = json["data"];
}

nlohmann::json DataEventCandlestick::toJson() const {
    throw std::runtime_error("Unimplemented: DataEventCandlestick::toJson()");
}

void DataEventCandlestick::fromJson(const nlohmann::json &json) {
    for (const auto &el: json.items()) {
        Candle candle;
        candle.fromJson(el.value());
        candles.push_back(candle);
    }
}

nlohmann::json DataEventTicker::toJson() const {
    throw std::runtime_error("Unimplemented: DataEventCandlestick::toJson()");
}

void DataEventTicker::fromJson(const nlohmann::json &json) {
    for (const auto &el: json.items()) {
        Ticker ticker;
        ticker.fromJson(el.value());
        tickers.push_back(ticker);
    }
}
}
