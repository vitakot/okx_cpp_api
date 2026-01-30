/**
OKX Market Data Utilities

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2025 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#ifndef INCLUDE_VK_OKX_MARKET_DATA_UTILS_H
#define INCLUDE_VK_OKX_MARKET_DATA_UTILS_H

#include "okx_models.h"
#include <vector>
#include <string>

namespace vk::okx::utils {

/**
 * Extract first file from ZIP archive stored in memory
 * @param zipData Raw ZIP file bytes
 * @return Decompressed file content
 * @throws std::runtime_error if ZIP extraction fails
 */
[[nodiscard]] std::vector<std::uint8_t> extractZip(const std::vector<std::uint8_t> &zipData);

/**
 * Parse 1-minute candlestick CSV data into Candle structures CSV format: ts,o,h,l,c,vol,volCcy,volCcyQuote,confirm
 * @param csvData Raw CSV bytes (UTF-8 encoded)
 * @return Vector of Candle structures
 * @throws std::runtime_error if CSV parsing fails
 */
[[nodiscard]] std::vector<Candle> parseCandlesCsv(const std::vector<std::uint8_t> &csvData);

/**
 * Parse 1-minute candlestick CSV data from string
 * @param csvContent CSV content as string
 * @return Vector of Candle structures
 */
[[nodiscard]] std::vector<Candle> parseCandlesCsv(const std::string &csvContent);

/**
 * Parse funding rate CSV data into FundingRate structures CSV format: instId,fundingRate,realizedRate,fundingTime
 * @param csvData Raw CSV bytes (UTF-8 encoded)
 * @return ector of FundingRate structures
 */
[[nodiscard]] std::vector<FundingRate> parseFundingRateCsv(const std::vector<std::uint8_t> &csvData);

} // namespace vk::okx::utils

#endif // INCLUDE_VK_OKX_MARKET_DATA_UTILS_H
