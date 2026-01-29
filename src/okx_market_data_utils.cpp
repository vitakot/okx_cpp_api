/**
OKX Market Data Utilities

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2025 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include "vk/okx/okx_market_data_utils.h"
#include <stdexcept>
#include <sstream>
#include <charconv>
#include <mz.h>
#include <mz_strm.h>
#include <mz_strm_mem.h>
#include <mz_zip.h>
#include <mz_zip_rw.h>
#include <spdlog/spdlog.h>

namespace vk::okx::utils {

std::vector<std::uint8_t> extractZip(const std::vector<std::uint8_t> &zipData) {
    // Create memory stream from input data
    void *mem_stream = mz_stream_mem_create();
    if (!mem_stream) {
        throw std::runtime_error("Failed to create memory stream");
    }

    mz_stream_mem_set_buffer(mem_stream, const_cast<void*>(static_cast<const void*>(zipData.data())),
                             static_cast<int32_t>(zipData.size()));
    mz_stream_open(mem_stream, nullptr, MZ_OPEN_MODE_READ);

    // Create zip reader
    void *zip_reader = mz_zip_reader_create();
    if (!zip_reader) {
        mz_stream_mem_delete(&mem_stream);
        throw std::runtime_error("Failed to create ZIP reader");
    }

    if (mz_zip_reader_open(zip_reader, mem_stream) != MZ_OK) {
        mz_zip_reader_delete(&zip_reader);
        mz_stream_mem_delete(&mem_stream);
        throw std::runtime_error("Failed to open ZIP archive");
    }

    // Go to first entry
    if (mz_zip_reader_goto_first_entry(zip_reader) != MZ_OK) {
        mz_zip_reader_close(zip_reader);
        mz_zip_reader_delete(&zip_reader);
        mz_stream_mem_delete(&mem_stream);
        throw std::runtime_error("ZIP archive is empty");
    }

    // Get file info
    mz_zip_file *file_info = nullptr;
    if (mz_zip_reader_entry_get_info(zip_reader, &file_info) != MZ_OK) {
        mz_zip_reader_close(zip_reader);
        mz_zip_reader_delete(&zip_reader);
        mz_stream_mem_delete(&mem_stream);
        throw std::runtime_error("Failed to get file info from ZIP");
    }

    // Allocate buffer for decompressed data
    std::vector<std::uint8_t> result(static_cast<size_t>(file_info->uncompressed_size));

    // Open and read entry
    if (mz_zip_reader_entry_open(zip_reader) != MZ_OK) {
        mz_zip_reader_close(zip_reader);
        mz_zip_reader_delete(&zip_reader);
        mz_stream_mem_delete(&mem_stream);
        throw std::runtime_error("Failed to open ZIP entry");
    }

    int32_t bytes_read = mz_zip_reader_entry_read(zip_reader, result.data(),
                                                   static_cast<int32_t>(result.size()));
    if (bytes_read < 0) {
        mz_zip_reader_entry_close(zip_reader);
        mz_zip_reader_close(zip_reader);
        mz_zip_reader_delete(&zip_reader);
        mz_stream_mem_delete(&mem_stream);
        throw std::runtime_error("Failed to extract file from ZIP");
    }

    result.resize(static_cast<size_t>(bytes_read));

    // Cleanup
    mz_zip_reader_entry_close(zip_reader);
    mz_zip_reader_close(zip_reader);
    mz_zip_reader_delete(&zip_reader);
    mz_stream_mem_delete(&mem_stream);

    return result;
}

std::vector<Candle> parseCandlesCsv(const std::vector<std::uint8_t> &csvData) {
    const std::string csvContent(reinterpret_cast<const char *>(csvData.data()), csvData.size());
    return parseCandlesCsv(csvContent);
}

std::vector<Candle> parseCandlesCsv(const std::string &csvContent) {
    std::vector<Candle> candles;
    std::istringstream stream(csvContent);
    std::string line;
    bool isFirstLine = true;
    int linesProcessed = 0;
    int linesSkipped = 0;

    while (std::getline(stream, line)) {
        // Handle Windows line endings (remove trailing \r)
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        // Skip empty lines
        if (line.empty()) {
            continue;
        }

        // Skip header line (may contain Chinese characters in legacy data)
        if (isFirstLine) {
            isFirstLine = false;
            // Check if this looks like a header (contains non-digit first character or known headers)
            if (!line.empty() && !std::isdigit(static_cast<unsigned char>(line[0]))) {
                continue;
            }
        }

        // Parse CSV line: ts,o,h,l,c,vol,volCcy,volCcyQuote,confirm
        std::istringstream lineStream(line);
        std::string field;
        std::vector<std::string> fields;

        while (std::getline(lineStream, field, ',')) {
            fields.push_back(field);
        }

        // OKX market data history CSV format (10 fields):
        // instrument_name,open,high,low,close,vol,vol_ccy,vol_quote,open_time,confirm
        if (fields.size() < 10) {
            if (linesSkipped < 5) {
                spdlog::warn("CSV line has {} fields (expected 10): {}", fields.size(), line);
            }
            linesSkipped++;
            continue; // Skip malformed lines
        }

        linesProcessed++;

        try {
            Candle candle;

            // fields[0] = instrument_name (skip)
            // fields[1] = open
            // fields[2] = high
            // fields[3] = low
            // fields[4] = close
            // fields[5] = vol
            // fields[6] = vol_ccy (may be "None")
            // fields[7] = vol_quote (may be "None")
            // fields[8] = open_time (timestamp)
            // fields[9] = confirm

            // Parse timestamp from open_time field
            std::int64_t ts = 0;
            auto [ptr, ec] = std::from_chars(fields[8].data(), fields[8].data() + fields[8].size(), ts);
            if (ec == std::errc()) {
                candle.ts = ts;
            }

            // Parse OHLC values
            candle.o = boost::multiprecision::cpp_dec_float_50(fields[1]);
            candle.h = boost::multiprecision::cpp_dec_float_50(fields[2]);
            candle.l = boost::multiprecision::cpp_dec_float_50(fields[3]);
            candle.c = boost::multiprecision::cpp_dec_float_50(fields[4]);

            // Parse volume - always present
            candle.vol = boost::multiprecision::cpp_dec_float_50(fields[5]);

            // Parse vol_ccy and vol_quote - may be "None"
            if (fields[6] != "None" && !fields[6].empty()) {
                candle.volCcy = boost::multiprecision::cpp_dec_float_50(fields[6]);
            } else {
                candle.volCcy = 0;
            }

            if (fields[7] != "None" && !fields[7].empty()) {
                candle.volCcyQuote = boost::multiprecision::cpp_dec_float_50(fields[7]);
            } else {
                candle.volCcyQuote = 0;
            }

            // Parse confirm flag
            candle.confirm = (fields[9] == "1" || fields[9] == "true" || fields[9] == "True");

            candles.push_back(candle);
        } catch (const std::exception &e) {
            // Log first few parse errors
            if (linesSkipped < 5) {
                spdlog::warn("Failed to parse CSV line: {} - error: {}", line, e.what());
            }
            linesSkipped++;
            continue;
        }
    }

    return candles;
}

std::vector<FundingRate> parseFundingRateCsv(const std::vector<std::uint8_t> &csvData) {
    const std::string csvContent(reinterpret_cast<const char *>(csvData.data()), csvData.size());

    std::vector<FundingRate> rates;
    std::istringstream stream(csvContent);
    std::string line;
    bool isFirstLine = true;

    while (std::getline(stream, line)) {
        if (line.empty()) {
            continue;
        }

        if (isFirstLine) {
            isFirstLine = false;
            if (!line.empty() && !std::isdigit(static_cast<unsigned char>(line[0]))) {
                continue;
            }
        }

        // Parse CSV line: instId,fundingRate,realizedRate,fundingTime
        std::istringstream lineStream(line);
        std::string field;
        std::vector<std::string> fields;

        while (std::getline(lineStream, field, ',')) {
            fields.push_back(field);
        }

        if (fields.size() < 4) {
            continue;
        }

        try {
            FundingRate rate;
            rate.instId = fields[0];
            rate.fundingRate = boost::multiprecision::cpp_dec_float_50(fields[1]);
            // fields[2] is realizedRate - not in our model

            std::int64_t fundingTime = 0;
            auto [ptr, ec] = std::from_chars(fields[3].data(), fields[3].data() + fields[3].size(), fundingTime);
            if (ec == std::errc()) {
                rate.fundingTime = fundingTime;
            }

            rates.push_back(rate);
        } catch (const std::exception &) {
            continue;
        }
    }

    return rates;
}

}
