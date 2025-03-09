/**
OKX Common Stuff

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2025 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#ifndef INCLUDE_VK_OKX_API_OKX_H
#define INCLUDE_VK_OKX_API_OKX_H

#include "vk/okx/okx_models.h"
#include "vk/okx/okx_event_models.h"
#include <magic_enum/magic_enum.hpp>

namespace vk::okx {
    class OKX {
    public:
        /**
         * Check if the input resolution in minutes is valid, if so then return corresponding API string
         * @param size Bar size in minutes.
         * @param barSize out: BarSize enum value
         * @return Tru if input resolution is valid
         */
        static bool isValidBarSize(std::int32_t size, BarSize &barSize);

        /**
         * Get number of ms for given Bar size
         * @param size
         * @return
         */
        static int64_t numberOfMsForBarSize(BarSize size);

        /**
         * Converts string to BarSize enum - must be done manually, enum values differs from string representations because
         * OKX naming does not comply to C++ conventions
         * @param barSize
         * @return
         */
        static std::optional<BarSize> stringToBarSize(const std::string &barSize);

        /**
         * Helper for converting BarSize, which is in the REST data models, to CandlestickChannel, which is
         * in the WS streams data models.
         * @param size
         * @return CandlestickChannel corresponding to BarSize value
         */
        static futures::CandlestickChannel barSizeToCandlestickChannel(BarSize size);

        /**
         * Helper for converting CandlestickChannel, which is in the WS streams data models, to BarSize, which is in the
         * REST data models
         * @param candlestickChannel
         * @return
         */
        static BarSize candlestickChannelToBarSize(futures::CandlestickChannel candlestickChannel);

        /**
         * Converts string to PositionSide enum - must be done manually, enum values differs from string representations because
         * OKX naming does not comply to C++ conventions
         * @param positionSide
         * @return
         */
        static std::optional<PositionSide> stringToPositionSide(const std::string &positionSide);
    };
}

template <>
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

template <>
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
#endif //INCLUDE_VK_OKX_API_OKX_H
