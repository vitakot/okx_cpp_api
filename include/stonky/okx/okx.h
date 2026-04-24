/**
OKX Common Stuff

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2025 Vitezslav Kot <vitezslav.kot@stonky.cz>, Stonky s.r.o.
*/

#ifndef INCLUDE_STONKY_OKX_API_OKX_H
#define INCLUDE_STONKY_OKX_API_OKX_H

#include "stonky/okx/okx_models.h"

namespace stonky::okx {
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
     * Helper for converting BarSize, which is in the REST data models, to CandlestickChannel, which is
     * in the WS streams data models.
     * @param size
     * @return CandlestickChannel corresponding to BarSize value
     */
    static CandlestickChannel barSizeToCandlestickChannel(BarSize size);

    /**
     * Helper for converting CandlestickChannel, which is in the WS streams data models, to BarSize, which is in the
     * REST data models
     * @param candlestickChannel
     * @return
     */
    static BarSize candlestickChannelToBarSize(CandlestickChannel candlestickChannel);
};
}
#endif //INCLUDE_STONKY_OKX_API_OKX_H
