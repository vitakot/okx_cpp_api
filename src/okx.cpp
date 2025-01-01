/**
OKX Common Stuff

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2025 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include "vk/okx/okx.h"

namespace vk::okx {
int64_t OKX::numberOfMsForBarSize(const BarSize size) {
    switch (size) {
        case BarSize::_1m:
            return 60000;
        case BarSize::_3m:
            return 60000 * 3;
        case BarSize::_5m:
            return 60000 * 5;
        case BarSize::_15m:
            return 60000 * 15;
        case BarSize::_30m:
            return 60000 * 30;
        case BarSize::_1H:
            return 60000 * 60;
        case BarSize::_2H:
            return 60000 * 120;
        case BarSize::_4H:
            return 60000 * 240;
        case BarSize::_6H:
            return 60000 * 360;
        case BarSize::_12H:
            return 60000 * 720;
        case BarSize::_1D:
            return 86400000;
        case BarSize::_2D:
            return 86400000 * 2;
        case BarSize::_3D:
            return 86400000 * 3;
        case BarSize::_1W:
            return 86400000 * 7;
        case BarSize::_1M:
            return static_cast<int64_t>(86400000) * 30;
        case BarSize::_3M:
            return static_cast<int64_t>(86400000) * 30 * 3;
        default:
            return 0;
    }
}

bool OKX::isValidBarSize(const std::int32_t size, BarSize &barSize) {
    switch (size) {
        case 1:
            barSize = BarSize::_1m;
            return true;
        case 3:
            barSize = BarSize::_3m;
            return true;
        case 5:
            barSize = BarSize::_5m;
            return true;
        case 15:
            barSize = BarSize::_15m;
            return true;
        case 30:
            barSize = BarSize::_30m;
            return true;
        case 60:
            barSize = BarSize::_1H;
            return true;
        case 120:
            barSize = BarSize::_2H;
            return true;
        case 240:
            barSize = BarSize::_4H;
            return true;
        case 360:
            barSize = BarSize::_6H;
            return true;
        case 720:
            barSize = BarSize::_12H;
            return true;
        case 1440:
            barSize = BarSize::_1D;
            return true;
        case 2880:
            barSize = BarSize::_2D;
            return true;
        case 4320:
            barSize = BarSize::_3D;
            return true;
        case 10080:
            barSize = BarSize::_1W;
            return true;
        case 40320:
            barSize = BarSize::_1M;
            return true;
        case 120960:
            barSize = BarSize::_3M;
            return true;
        default:
            return false;
    }
}

std::optional<BarSize> OKX::stringToBarSize(const std::string &barSize) {
    if (barSize == "1m") {
        return BarSize::_1m;
    }
    if (barSize == "3m") {
        return BarSize::_3m;
    }
    if (barSize == "5m") {
        return BarSize::_5m;
    }
    if (barSize == "15m") {
        return BarSize::_15m;
    }
    if (barSize == "30m") {
        return BarSize::_30m;
    }
    if (barSize == "1H") {
        return BarSize::_1H;
    }
    if (barSize == "2H") {
        return BarSize::_2H;
    }
    if (barSize == "4H") {
        return BarSize::_4H;
    }
    if (barSize == "6H") {
        return BarSize::_6H;
    }
    if (barSize == "12H") {
        return BarSize::_12H;
    }
    if (barSize == "1D") {
        return BarSize::_1D;
    }
    if (barSize == "2D") {
        return BarSize::_2D;
    }
    if (barSize == "3D") {
        return BarSize::_3D;
    }
    if (barSize == "1W") {
        return BarSize::_1W;
    }
    if (barSize == "1M") {
        return BarSize::_1M;
    }
    if (barSize == "3M") {
        return BarSize::_3M;
    }

    return {};
}

futures::CandlestickChannel OKX::barSizeToCandlestickChannel(const BarSize size) {
    switch (size) {
        case BarSize::_1m:
            return futures::CandlestickChannel::candle1m;
        case BarSize::_3m:
            return futures::CandlestickChannel::candle3m;
        case BarSize::_5m:
            return futures::CandlestickChannel::candle5m;
        case BarSize::_15m:
            return futures::CandlestickChannel::candle15m;
        case BarSize::_30m:
            return futures::CandlestickChannel::candle30m;
        case BarSize::_1H:
            return futures::CandlestickChannel::candle1H;
        case BarSize::_2H:
            return futures::CandlestickChannel::candle2H;
        case BarSize::_4H:
            return futures::CandlestickChannel::candle4H;
        case BarSize::_6H:
            return futures::CandlestickChannel::candle6H;
        case BarSize::_12H:
            return futures::CandlestickChannel::candle12H;
        case BarSize::_1D:
            return futures::CandlestickChannel::candle1D;
        case BarSize::_2D:
            return futures::CandlestickChannel::candle2D;
        case BarSize::_3D:
            return futures::CandlestickChannel::candle3D;
        case BarSize::_1W:
            return futures::CandlestickChannel::candle1W;
        case BarSize::_1M:
            return futures::CandlestickChannel::candle1M;
        case BarSize::_3M:
            return futures::CandlestickChannel::candle3M;
        default:
            return futures::CandlestickChannel::candle1H;
    }
}

BarSize OKX::candlestickChannelToBarSize(const futures::CandlestickChannel candlestickChannel) {
    switch (candlestickChannel) {
        case futures::CandlestickChannel::candle1m:
            return BarSize::_1m;
        case futures::CandlestickChannel::candle3m:
            return BarSize::_3m;
        case futures::CandlestickChannel::candle5m:
            return BarSize::_5m;
        case futures::CandlestickChannel::candle15m:
            return BarSize::_15m;
        case futures::CandlestickChannel::candle30m:
            return BarSize::_30m;
        case futures::CandlestickChannel::candle1H:
            return BarSize::_1H;
        case futures::CandlestickChannel::candle2H:
            return BarSize::_2H;
        case futures::CandlestickChannel::candle4H:
            return BarSize::_4H;
        case futures::CandlestickChannel::candle6H:
            return BarSize::_6H;
        case futures::CandlestickChannel::candle12H:
            return BarSize::_12H;
        case futures::CandlestickChannel::candle1D:
            return BarSize::_1D;
        case futures::CandlestickChannel::candle2D:
            return BarSize::_2D;
        case futures::CandlestickChannel::candle3D:
            return BarSize::_3D;
        case futures::CandlestickChannel::candle1W:
            return BarSize::_1W;
        case futures::CandlestickChannel::candle1M:
            return BarSize::_1M;
        case futures::CandlestickChannel::candle3M:
            return BarSize::_3M;
        default:
            return BarSize::_1H;
    }
}

std::optional<PositionSide> OKX::stringToPositionSide(const std::string &positionSide) {
    if (positionSide == "long") {
        return PositionSide::_long;
    }
    if (positionSide == "short") {
        return PositionSide::_short;
    }
    if (positionSide == "net") {
        return PositionSide::_net;
    }

    return {};
}
}
