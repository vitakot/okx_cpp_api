/**
OKX Enums

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2026 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#ifndef INCLUDE_VK_OKX_ENUMS_H
#define INCLUDE_VK_OKX_ENUMS_H

#include "vk/utils/magic_enum_wrapper.hpp"

namespace vk::okx {
enum class InstrumentType : std::int32_t {
    SPOT,
    SWAP,
    FUTURES,
    OPTION,
    MARGIN
};

/// BarSize is not BETTER_ENUM because the vales have to be converted into string manually, API works with values
/// without underscores which are not C++ compliant
enum class BarSize : std::int32_t {
    _1m,
    _3m,
    _5m,
    _15m,
    _30m,
    _1H,
    _2H,
    _4H,
    _6H,
    _12H,
    _1D,
    _2D,
    _3D,
    _1W,
    _1M,
    _3M
};

enum class OptionType : std::int32_t {
    C,
    P
};

enum class ContractType : std::int32_t {
    linear,
    inverse
};

enum class FuturesAlias : std::int32_t {
    this_week,
    next_week,
    quarter,
    next_quarter
};

enum class InstrumentStatus : std::int32_t {
    live,
    suspend,
    preopen,
    test
};

enum class MarginMode : std::int32_t {
    cross,
    isolated,
    cash
};

enum class OrderType : std::int32_t {
    market,
    limit,
    post_only,
    fok,
    ioc,
    optimal_limit_ioc
};


enum class Side : std::int32_t {
    buy,
    sell
};

enum class OrderState : std::int32_t {
    canceled,
    live,
    partially_filled,
    filled
};

/// PositionSide is not BETTER_ENUM because the values are C++ keywords and so must be converted into string manually
enum class PositionSide : std::int32_t {
    _long,
    _short,
    _net
};

enum class OperationType : std::int32_t {
    subscribe,
    unsubscribe
};

enum class SettState  : std::int32_t {
    processing,
    settled
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

/// Market data module type for historical data endpoint
enum class MarketDataModule : std::int32_t {
    Trades = 1,
    Candles1m = 2,
    FundingRate = 3,
    Orderbook400 = 4,
    Orderbook5000 = 5,
    Orderbook50 = 6
};

/// Date aggregation type for historical data
enum class DateAggrType : std::int32_t {
    daily,
    monthly
};
}

template<>
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<vk::okx::BarSize>(
    const vk::okx::BarSize value) noexcept {
    switch (value) {
        case vk::okx::BarSize::_1m:
            return "1m";
        case vk::okx::BarSize::_3m:
            return "3m";
        case vk::okx::BarSize::_5m:
            return "5m";
        case vk::okx::BarSize::_15m:
            return "15m";
        case vk::okx::BarSize::_30m:
            return "30m";
        case vk::okx::BarSize::_1H:
            return "1H";
        case vk::okx::BarSize::_2H:
            return "2H";
        case vk::okx::BarSize::_4H:
            return "4H";
        case vk::okx::BarSize::_6H:
            return "6H";
        case vk::okx::BarSize::_12H:
            return "12H";
        case vk::okx::BarSize::_1D:
            return "1D";
        case vk::okx::BarSize::_2D:
            return "2D";
        case vk::okx::BarSize::_3D:
            return "3D";
        case vk::okx::BarSize::_1W:
            return "1W";
        case vk::okx::BarSize::_1M:
            return "1M";
        case vk::okx::BarSize::_3M:
            return "3M";
    }

    return default_tag;
}

template<>
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<vk::okx::PositionSide>(
    const vk::okx::PositionSide value) noexcept {
    switch (value) {
        case vk::okx::PositionSide::_long:
            return "long";
        case vk::okx::PositionSide::_short:
            return "short";
        case vk::okx::PositionSide::_net:
            return "net";
    }

    return default_tag;
}
#endif //INCLUDE_VK_OKX_ENUMS_H
