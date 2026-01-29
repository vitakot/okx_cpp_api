# okx_cpp_api

C++ connector library for OKX cryptocurrency exchange API v5.

## Features

- REST API client for futures trading
- WebSocket client for real-time market data
- Historical market data bulk download with ZIP extraction
- Support for SPOT, SWAP, FUTURES, and OPTION instruments

## Requirements

- C++20 compiler
- CMake 3.20+
- Boost 1.83+ (ASIO, Beast)
- OpenSSL
- nlohmann_json
- spdlog
- magic_enum

## Building

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Usage Examples

### REST Client - Basic Operations

```cpp
#include "vk/okx/okx_futures_rest_client.h"

using namespace vk::okx::futures;

// Create client (empty credentials for public endpoints)
RESTClient client("", "", "");

// Get instruments
auto instruments = client.getInstruments(InstrumentType::SWAP);

// Get tickers
auto tickers = client.getTickers(InstrumentType::SWAP);

// Get historical candles (paginated, may be interrupted)
auto candles = client.getHistoricalPrices(
    "BTC-USDT-SWAP",
    BarSize::_1H,
    fromTimestamp,
    toTimestamp);
```

### Historical Market Data Bulk Download

New robust method for downloading historical data via ZIP files:

```cpp
#include "vk/okx/okx_futures_rest_client.h"

using namespace vk::okx::futures;

RESTClient client("", "", "");

// Option 1: Get download URLs and process manually
auto history = client.getMarketDataHistory(
    MarketDataModule::Candles1m,  // 1-minute candles
    InstrumentType::SWAP,
    "BTC-USDT",
    DateAggrType::daily,
    beginTimestamp,
    endTimestamp);

for (const auto& detail : history.details) {
    for (const auto& file : detail.groupDetails) {
        std::cout << "File: " << file.filename
                  << " (" << file.sizeMB << " MB)" << std::endl;

        // Download ZIP file
        auto zipData = client.downloadMarketDataFile(file.url);
        // Process zipData...
    }
}

// Option 2: High-level method - download, extract, and parse automatically
auto candles = client.downloadAndParseHistoricalCandles(
    InstrumentType::SWAP,
    "BTC-USDT",
    DateAggrType::daily,
    beginTimestamp,
    endTimestamp);

std::cout << "Downloaded " << candles.size() << " candles" << std::endl;
```

### Available Data Modules

| Module | Enum Value | Description |
|--------|------------|-------------|
| 1 | `MarketDataModule::Trades` | Tick-by-tick trade history |
| 2 | `MarketDataModule::Candles1m` | 1-minute OHLC candlesticks |
| 3 | `MarketDataModule::FundingRate` | Funding rate history |
| 4 | `MarketDataModule::Orderbook400` | 400-level order book |
| 5 | `MarketDataModule::Orderbook5000` | 5000-level order book |
| 6 | `MarketDataModule::Orderbook50` | 50-level order book (deprecated) |

### Date Aggregation Types

- `DateAggrType::daily` - Daily files (max range: 20 days)
- `DateAggrType::monthly` - Monthly files (max range: 20 months)

## Project Structure

```
okx_cpp_api/
├── include/vk/okx/
│   ├── okx_futures_rest_client.h    # REST API client
│   ├── okx_futures_ws_client.h      # WebSocket client
│   ├── okx_http_session.h           # HTTP/HTTPS session
│   ├── okx_models.h                 # Data models
│   ├── okx_market_data_utils.h      # ZIP/CSV utilities
│   └── ...
├── src/
│   ├── okx_futures_rest_client.cpp
│   ├── okx_http_session.cpp
│   ├── okx_market_data_utils.cpp
│   └── ...
├── vk_cpp_common/                   # Common utilities submodule
└── test/
    └── main.cpp
```

## License

MIT License - see source files for details.

## Author

Vitezslav Kot <vitezslav.kot@gmail.com>
