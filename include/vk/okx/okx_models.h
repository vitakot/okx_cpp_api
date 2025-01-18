/**
OKX Data Models

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2025 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#ifndef INCLUDE_VK_OKX_MODELS_H
#define INCLUDE_VK_OKX_MODELS_H

#include "vk/interface/i_json.h"
#include <nlohmann/json.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>

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

struct Response : IJson {
    std::string m_code{};
    std::string m_msg{};
    nlohmann::json m_data{};

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct Ticker final : IJson {
    InstrumentType m_instType{InstrumentType::SPOT};

    /// Instrument ID
    std::string m_instId{};

    /// Last traded price
    boost::multiprecision::cpp_dec_float_50 m_last{};

    /// Last traded size
    boost::multiprecision::cpp_dec_float_50 m_lastSz{};

    /// Best ask price
    boost::multiprecision::cpp_dec_float_50 m_askPx{};

    /// Best ask size
    boost::multiprecision::cpp_dec_float_50 m_askSz{};

    /// Best bid price
    boost::multiprecision::cpp_dec_float_50 m_bidPx{};

    /// Best bid size
    boost::multiprecision::cpp_dec_float_50 m_bidSz{};

    /// Open price in the past 24 hours
    boost::multiprecision::cpp_dec_float_50 m_open24h{};

    /// Highest price in the past 24 hours
    boost::multiprecision::cpp_dec_float_50 m_high24h{};

    /// Lowest price in the past 24 hours
    boost::multiprecision::cpp_dec_float_50 m_low24h{};

    /// 24h trading volume, with a unit of currency. If it is a derivatives contract, the value is the number
    /// of base currency. If it is SPOT/MARGIN, the value is the quantity in quote currency.
    boost::multiprecision::cpp_dec_float_50 m_volCcy24h{};

    /// 24h trading volume, with a unit of contract. If it is a derivatives contract, the value is the number of
    /// contracts. If it is SPOT/MARGIN, the value is the quantity in base currency.
    boost::multiprecision::cpp_dec_float_50 m_vol24h{};

    /// Open price in the UTC 0
    boost::multiprecision::cpp_dec_float_50 m_sodUtc0{};

    /// Open price in the UTC 8
    boost::multiprecision::cpp_dec_float_50 m_sodUtc8{};

    /// Ticker data generation time, Unix timestamp format in milliseconds
    std::int64_t m_ts{};

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct Tickers final : Response {
    std::vector<Ticker> m_tickers{};

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct Candle final : IJson {
    /// Opening time of the candlestick
    std::int64_t m_ts{};
    boost::multiprecision::cpp_dec_float_50 m_o{};
    boost::multiprecision::cpp_dec_float_50 m_h{};
    boost::multiprecision::cpp_dec_float_50 m_l{};
    boost::multiprecision::cpp_dec_float_50 m_c{};
    boost::multiprecision::cpp_dec_float_50 m_vol{};

    /// Trading volume, with a unit of currency. If it is a derivatives contract, the value is the number of base
    /// currency. If it is SPOT/MARGIN, the value is the quantity in quote currency.
    boost::multiprecision::cpp_dec_float_50 m_volCcy{};

    /// Trading volume, the value is the quantity in quote currency, e.g. the unit is USDT for BTC-USDT and BTC-USDT-SWAP;
    /// The unit is USD for BTC-USD-SWAP
    boost::multiprecision::cpp_dec_float_50 m_volCcyQuote{};

    /// The state of candlesticks. False represents that it is uncompleted, True represents that it is completed.
    bool m_confirm{};

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct Candles final : Response {
    std::vector<Candle> m_candles{};

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct FundingRate final : Response {
    InstrumentType m_instType{InstrumentType::SPOT};
    std::string m_instId{};
    boost::multiprecision::cpp_dec_float_50 m_fundingRate{};
    std::int64_t m_fundingTime{};
    std::int64_t m_nextFundingTime{};
    boost::multiprecision::cpp_dec_float_50 m_nextFundingRate{};

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct FundingRates final : Response {
    std::vector<FundingRate> m_rates{};

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct Instrument final : IJson {
    InstrumentType m_instType{InstrumentType::SPOT};

    /// Instrument ID, e.g. BTC-USD-SWAP
    std::string m_instId{};

    /// Instrument family, e.g. BTC-USD, only applicable to FUTURES/SWAP/OPTION
    std::string m_instFamily{};

    /// Underlying, e.g. BTC-USD, only applicable to FUTURES/SWAP/OPTION
    std::string m_uly{};

    /// Base currency, e.g. BTC inBTC-USDT, only applicable to SPOT/MARGIN
    std::string m_baseCcy{};

    /// Quote currency, e.g. USDT in BTC-USDT, only applicable to SPOT/MARGIN
    std::string m_quoteCcy{};

    /// Settlement and margin currency, e.g. BTC, only applicable to FUTURES/SWAP/OPTION
    std::string m_settleCcy{};

    /// Contract value, only applicable to FUTURES/SWAP/OPTION
    boost::multiprecision::cpp_dec_float_50 m_ctVal{};

    /// Contract multiplier, only applicable to FUTURES/SWAP/OPTION
    boost::multiprecision::cpp_dec_float_50 m_ctMult{};

    /// Contract value currency, only applicable to FUTURES/SWAP/OPTIO
    std::string m_ctValCcy{};

    /// Option type, C: Call P: put, only applicable to OPTION
    OptionType m_optType{OptionType::C};

    /// Strike price, only applicable to OPTION
    boost::multiprecision::cpp_dec_float_50 m_stk{};

    /// Listing time, Unix timestamp format in milliseconds
    std::int64_t m_listTime{};

    /// Expiry time, Unix timestamp format in milliseconds
    std::int64_t m_expTime{};

    /// Max Leverage, not applicable to SPOT, OPTION
    boost::multiprecision::cpp_dec_float_50 m_lever{};

    /// Tick size, e.g. 0.0001
    boost::multiprecision::cpp_dec_float_50 m_tickSz{};

    /// Lot size, e.g. BTC-USDT-SWAP: 1
    boost::multiprecision::cpp_dec_float_50 m_lotSz{};

    /// Minimum order size. If it is a derivatives contract, the value is the number of contracts. If it is SPOT/MARGIN,
    /// the value is the quantity in base currency
    boost::multiprecision::cpp_dec_float_50 m_minSz{};

    /// Contract type, linear: linear contract, inverse: inverse contract, only applicable to FUTURES/SWAP
    ContractType m_ctType{ContractType::linear};

    /// Alias, only applicable to FUTURES
    FuturesAlias m_alias = {FuturesAlias::this_week};

    /// Instrument status
    InstrumentStatus m_state = {InstrumentStatus::live};

    /// The maximum order quantity of the contract or spot limit order. If it is a derivatives contract, the value
    /// is the number of contracts. If it is SPOT/MARGIN, the value is the quantity in base currency
    boost::multiprecision::cpp_dec_float_50 m_maxLmtSz{};

    /// The maximum order quantity of the contract or spot market order. If it is a derivatives contract, the value is
    /// the number of contracts. If it is SPOT/MARGIN, the value is the quantity in "USDT
    boost::multiprecision::cpp_dec_float_50 m_maxMktSz{};

    /// The maximum order quantity of the contract or spot twap order. If it is a derivatives contract, the value is
    /// the number of contracts. If it is SPOT/MARGIN, the value is the quantity in base currency.
    boost::multiprecision::cpp_dec_float_50 m_maxTwapSz{};

    /// The maximum order quantity of the contract or spot iceBerg order. If it is a derivatives contract, the value
    /// is the number of contracts. If it is SPOT/MARGIN, the value is the quantity in base currency.
    boost::multiprecision::cpp_dec_float_50 m_maxIcebergSz{};

    /// The maximum order quantity of the contract or spot trigger order. If it is a derivatives contract, the value is
    /// the number of contracts. If it is SPOT/MARGIN, the value is the quantity in base currency.
    boost::multiprecision::cpp_dec_float_50 m_maxTriggerSz{};

    /// The maximum order quantity of the contract or spot stop market order. If it is a derivatives contract, the
    /// value is the number of contracts. If it is SPOT/MARGIN, the value is the quantity in "USDT".
    boost::multiprecision::cpp_dec_float_50 m_maxStopSz{};

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct Instruments final : Response {
    std::vector<Instrument> m_instruments{};

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct BalanceDetail final : IJson {
    boost::multiprecision::cpp_dec_float_50 m_availBal{};
    boost::multiprecision::cpp_dec_float_50 m_availEq{};
    boost::multiprecision::cpp_dec_float_50 m_cashBal{};
    std::string m_ccy{};
    boost::multiprecision::cpp_dec_float_50 m_crossLiab{};
    boost::multiprecision::cpp_dec_float_50 m_disEq{};
    boost::multiprecision::cpp_dec_float_50 m_eq{};
    boost::multiprecision::cpp_dec_float_50 m_eqUsd{};
    boost::multiprecision::cpp_dec_float_50 m_frozenBal{};
    boost::multiprecision::cpp_dec_float_50 m_interest{};
    boost::multiprecision::cpp_dec_float_50 m_isoEq{};
    boost::multiprecision::cpp_dec_float_50 m_isoLiab{};
    boost::multiprecision::cpp_dec_float_50 m_isoUpl{};
    boost::multiprecision::cpp_dec_float_50 m_liab{};
    boost::multiprecision::cpp_dec_float_50 m_maxLoan{};
    boost::multiprecision::cpp_dec_float_50 m_mgnRatio{};
    boost::multiprecision::cpp_dec_float_50 m_notionalLever{};
    boost::multiprecision::cpp_dec_float_50 m_ordFrozen{};
    boost::multiprecision::cpp_dec_float_50 m_twap{};
    boost::multiprecision::cpp_dec_float_50 m_upl{};
    std::int64_t m_uTime{};
    boost::multiprecision::cpp_dec_float_50 m_uplLiab{};
    boost::multiprecision::cpp_dec_float_50 m_stgyEq{};
    boost::multiprecision::cpp_dec_float_50 m_spotInUseAmt{};

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct Balance final : Response {
    boost::multiprecision::cpp_dec_float_50 m_adjEq{};
    boost::multiprecision::cpp_dec_float_50 m_imr{};
    boost::multiprecision::cpp_dec_float_50 m_isoEq{};
    boost::multiprecision::cpp_dec_float_50 m_mgnRatio{};
    boost::multiprecision::cpp_dec_float_50 m_mmr{};
    boost::multiprecision::cpp_dec_float_50 m_notionalUsd{};
    boost::multiprecision::cpp_dec_float_50 m_ordFroz{};
    boost::multiprecision::cpp_dec_float_50 m_totalEq{};
    std::int64_t m_uTime{};

    std::vector<BalanceDetail> m_balanceDetails{};

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct SystemTime final : Response {
    std::int64_t m_ts{};

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct Position final : IJson {
    std::int32_t m_adl{};
    boost::multiprecision::cpp_dec_float_50 m_availPos{};
    boost::multiprecision::cpp_dec_float_50 m_avgPx{};
    std::int64_t m_cTime{};
    std::string m_ccy{};
    boost::multiprecision::cpp_dec_float_50 m_imr{};
    std::string m_instId{};
    InstrumentType m_instType{InstrumentType::MARGIN};
    boost::multiprecision::cpp_dec_float_50 m_interest{};
    boost::multiprecision::cpp_dec_float_50 m_last{};
    boost::multiprecision::cpp_dec_float_50 m_lever{};
    boost::multiprecision::cpp_dec_float_50 m_liab{};
    std::string m_liabCcy{};
    boost::multiprecision::cpp_dec_float_50 m_liqPx{};
    boost::multiprecision::cpp_dec_float_50 m_margin{};
    boost::multiprecision::cpp_dec_float_50 m_markPx{};
    MarginMode m_mgnMode{MarginMode::cross};
    boost::multiprecision::cpp_dec_float_50 m_mgnRatio{};
    boost::multiprecision::cpp_dec_float_50 m_mmr{};
    boost::multiprecision::cpp_dec_float_50 m_notionalUsd{};
    boost::multiprecision::cpp_dec_float_50 m_pos{};
    std::string m_posCcy{};
    std::string m_posId{};
    PositionSide m_posSide{PositionSide::_net};
    std::string m_tradeId{};
    std::int64_t m_uTime{};
    boost::multiprecision::cpp_dec_float_50 m_upl{};
    boost::multiprecision::cpp_dec_float_50 m_uplRatio{};

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct Positions final : Response {
    std::vector<Position> m_positions{};

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct Order final : IJson {
    std::string m_instId{};
    MarginMode m_tdMode{MarginMode::cross};
    std::string m_clOrdId{};
    Side m_side{Side::buy};
    std::string m_ccy{};
    PositionSide m_posSide{PositionSide::_net};
    OrderType m_ordType{OrderType::market};
    boost::multiprecision::cpp_dec_float_50 m_sz{};
    boost::multiprecision::cpp_dec_float_50 m_px{};

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct OrderResponse final : IJson {
    std::string m_clOrdId{};
    std::string m_ordId{};
    std::string m_tag{};
    std::string m_sCode{};
    std::string m_sMsg{};

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct OrderResponses final : Response {
    std::vector<OrderResponse> m_orderResponses{};

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct OrderDetail final : IJson {
    InstrumentType m_instType{InstrumentType::MARGIN};
    std::string m_instId{};
    std::string m_ccy{};
    std::string m_ordId{};
    std::string m_clOrdId{};
    boost::multiprecision::cpp_dec_float_50 m_px{};
    boost::multiprecision::cpp_dec_float_50 m_sz{};
    boost::multiprecision::cpp_dec_float_50 m_pnl{};
    OrderType m_ordType{OrderType::market};
    Side m_side{Side::buy};
    PositionSide m_posSide{PositionSide::_long};
    MarginMode m_tdMode = MarginMode::isolated;
    boost::multiprecision::cpp_dec_float_50 m_accFillSz{};
    boost::multiprecision::cpp_dec_float_50 m_fillPx{};
    std::string m_tradeId{};
    boost::multiprecision::cpp_dec_float_50 m_fillSz{};
    std::int64_t m_fillTime{};
    OrderState m_state{OrderState::live};
    boost::multiprecision::cpp_dec_float_50 m_avgPx{};
    boost::multiprecision::cpp_dec_float_50 m_lever{};
    std::int64_t m_uTime{};
    std::int64_t m_cTime{};

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};

struct OrderDetails final : Response {
    std::vector<OrderDetail> m_orderDetails{};

    [[nodiscard]] nlohmann::json toJson() const override;

    void fromJson(const nlohmann::json &json) override;
};
}

#endif //INCLUDE_VK_OKX_MODELS_H
