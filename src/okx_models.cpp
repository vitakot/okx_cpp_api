/**
OKX Data Models

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2025 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include "vk/okx/okx_models.h"
#include "vk/utils/utils.h"
#include "vk/utils/json_utils.h"
#include "vk/okx/okx.h"
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <utility>

namespace vk::okx {
bool
readDecimalValue(const nlohmann::json &json, const std::string &key, boost::multiprecision::cpp_dec_float_50 &value,
                 boost::multiprecision::cpp_dec_float_50 defaultVal = boost::multiprecision::cpp_dec_float_50("0")) {
    if (const auto it = json.find(key); it != json.end()) {
        if (!it.value().is_null() && it->is_string() && !it->get<std::string>().empty()) {
            value.assign(it->get<std::string>());
            return true;
        }
        value = std::move(defaultVal);
    }

    return false;
}

nlohmann::json Response::toJson() const {
    nlohmann::json json;
    json["code"] = m_code;
    json["msg"] = m_msg;
    return json;
}

void Response::fromJson(const nlohmann::json &json) {
    readValue<std::string>(json, "code", m_code);
    readValue<std::string>(json, "msg", m_msg);
    m_data = json["data"];
}

nlohmann::json Ticker::toJson() const {
    throw std::runtime_error("Unimplemented: Ticker::toJson()");
}

void Ticker::fromJson(const nlohmann::json &json) {
    readMagicEnum<InstrumentType>(json, "instType", m_instType);
    readValue<std::string>(json, "instId", m_instId);
    readDecimalValue(json, "last", m_last);
    readDecimalValue(json, "lastSz", m_lastSz);
    readDecimalValue(json, "askPx", m_askPx);
    readDecimalValue(json, "askSz", m_askSz);
    readDecimalValue(json, "bidPx", m_bidPx);
    readDecimalValue(json, "bidSz", m_bidSz);
    readDecimalValue(json, "open24h", m_open24h);
    readDecimalValue(json, "high24h", m_high24h);
    readDecimalValue(json, "low24h", m_low24h);
    readDecimalValue(json, "volCcy24h", m_volCcy24h);
    readDecimalValue(json, "vol24h", m_vol24h);
    readDecimalValue(json, "sodUtc0", m_sodUtc0);
    readDecimalValue(json, "sodUtc8", m_sodUtc8);
    readDecimalValue(json, "vol24h", m_vol24h);
    m_ts = readStringAsInt64(json, "ts");
}

nlohmann::json Tickers::toJson() const {
    throw std::runtime_error("Unimplemented: Tickers::toJson()");
}

void Tickers::fromJson(const nlohmann::json &json) {
    Response::fromJson(json);

    for (const auto &el: m_data.items()) {
        Ticker ticker;
        ticker.fromJson(el.value());
        m_tickers.push_back(ticker);
    }
}

nlohmann::json Candle::toJson() const {
    throw std::runtime_error("Unimplemented: Candle::toJson()");
}

void Candle::fromJson(const nlohmann::json &json) {
    m_ts = stoll(json[0].get<std::string>());
    m_o = boost::multiprecision::cpp_dec_float_50(json[1].get<std::string>());
    m_h = boost::multiprecision::cpp_dec_float_50(json[2].get<std::string>());
    m_l = boost::multiprecision::cpp_dec_float_50(json[3].get<std::string>());
    m_c = boost::multiprecision::cpp_dec_float_50(json[4].get<std::string>());
    m_vol = boost::multiprecision::cpp_dec_float_50(json[5].get<std::string>());
    m_volCcy = boost::multiprecision::cpp_dec_float_50(json[6].get<std::string>());
    m_volCcyQuote = boost::multiprecision::cpp_dec_float_50(json[7].get<std::string>());
    m_confirm = string2bool(json[8].get<std::string>());
}

nlohmann::json Candles::toJson() const {
    throw std::runtime_error("Unimplemented: Candles::toJson()");
}

void Candles::fromJson(const nlohmann::json &json) {
    Response::fromJson(json);

    for (const auto &el: m_data.items()) {
        Candle candle;
        candle.fromJson(el.value());
        m_candles.push_back(candle);
    }
}

nlohmann::json FundingRate::toJson() const {
    throw std::runtime_error("Unimplemented: FundingRate::toJson()");
}

void FundingRate::fromJson(const nlohmann::json &json) {
    if (json.contains("data")) {
        Response::fromJson(json);
        if (!m_data.empty()) {
            m_data = m_data[0];
        }
    } else {
        m_data = json;
    }

    if (!m_data.empty()) {
        readMagicEnum<InstrumentType>(m_data, "instType", m_instType);
        readValue<std::string>(m_data, "instId", m_instId);
        readDecimalValue(m_data, "fundingRate", m_fundingRate);
        m_fundingTime = readStringAsInt64(m_data, "fundingTime");
        m_nextFundingTime = readStringAsInt64(m_data, "nextFundingTime");
        readDecimalValue(m_data, "nextFundingRate", m_nextFundingRate);
    }
}

nlohmann::json FundingRates::toJson() const {
    throw std::runtime_error("Unimplemented: FundingRates::toJson()");
}

void FundingRates::fromJson(const nlohmann::json &json) {
    Response::fromJson(json);

    for (const auto &el: m_data.items()) {
        FundingRate fundingRate;
        fundingRate.fromJson(el.value());
        m_rates.push_back(fundingRate);
    }
}

nlohmann::json Instrument::toJson() const {
    throw std::runtime_error("Unimplemented: Instrument::toJson()");
}

void Instrument::fromJson(const nlohmann::json &json) {
    readMagicEnum<InstrumentType>(json, "instType", m_instType);
    readValue<std::string>(json, "instId", m_instId);
    readValue<std::string>(json, "instFamily", m_instFamily);
    readValue<std::string>(json, "uly", m_uly);
    readValue<std::string>(json, "baseCcy", m_baseCcy);
    readValue<std::string>(json, "quoteCcy", m_quoteCcy);
    readValue<std::string>(json, "settleCcy", m_settleCcy);
    readDecimalValue(json, "ctVal", m_ctVal);
    readDecimalValue(json, "ctMult", m_ctMult);
    readValue<std::string>(json, "ctValCcy", m_ctValCcy);
    readMagicEnum<OptionType>(json, "optType", m_optType);
    readDecimalValue(json, "stk", m_stk);
    m_listTime = readStringAsInt64(json, "listTime");
    m_expTime = readStringAsInt64(json, "expTime");
    readDecimalValue(json, "lever", m_lever);
    readDecimalValue(json, "tickSz", m_tickSz);
    readDecimalValue(json, "lotSz", m_lotSz);
    readDecimalValue(json, "minSz", m_minSz);
    readMagicEnum<ContractType>(json, "ctType", m_ctType);
    readMagicEnum<FuturesAlias>(json, "alias", m_alias);
    readMagicEnum<InstrumentStatus>(json, "state", m_state);
    readDecimalValue(json, "maxLmtSz", m_maxLmtSz);
    readDecimalValue(json, "maxMktSz", m_maxMktSz);
    readDecimalValue(json, "maxTwapSz", m_maxTwapSz);
    readDecimalValue(json, "maxIcebergSz", m_maxIcebergSz);
    readDecimalValue(json, "maxTriggerSz", m_maxTriggerSz);
    readDecimalValue(json, "maxStopSz", m_maxStopSz);
}

nlohmann::json Instruments::toJson() const {
    throw std::runtime_error("Unimplemented: Instruments::toJson()");
}

void Instruments::fromJson(const nlohmann::json &json) {
    Response::fromJson(json);

    for (const auto &el: m_data.items()) {
        Instrument instrument;
        instrument.fromJson(el.value());
        m_instruments.push_back(instrument);
    }
}

nlohmann::json BalanceDetail::toJson() const {
    throw std::runtime_error("Unimplemented: Balance::toJson()");
}

void BalanceDetail::fromJson(const nlohmann::json &json) {
    readDecimalValue(json, "availBal", m_availBal);
    readDecimalValue(json, "availEq", m_availEq);
    readDecimalValue(json, "cashBal", m_cashBal);
    readValue<std::string>(json, "ccy", m_ccy);
    readDecimalValue(json, "crossLiab", m_crossLiab);
    readDecimalValue(json, "disEq", m_disEq);
    readDecimalValue(json, "eq", m_eq);
    readDecimalValue(json, "eqUsd", m_eqUsd);
    readDecimalValue(json, "frozenBal", m_frozenBal);
    readDecimalValue(json, "interest", m_interest);
    readDecimalValue(json, "isoEq", m_isoEq);
    readDecimalValue(json, "isoLiab", m_isoLiab);
    readDecimalValue(json, "isoUpl", m_isoUpl);
    readDecimalValue(json, "liab", m_liab);
    readDecimalValue(json, "maxLoan", m_maxLoan);
    readDecimalValue(json, "mgnRatio", m_mgnRatio);
    readDecimalValue(json, "notionalLever", m_notionalLever);
    readDecimalValue(json, "ordFrozen", m_ordFrozen);
    readDecimalValue(json, "twap", m_twap);
    readDecimalValue(json, "upl", m_upl);
    m_uTime = readStringAsInt64(json, "uTime");
    readDecimalValue(json, "uplLiab", m_uplLiab);
    readDecimalValue(json, "stgyEq", m_stgyEq);
    readDecimalValue(json, "spotInUseAmt", m_spotInUseAmt);
}

nlohmann::json Balance::toJson() const {
    throw std::runtime_error("Unimplemented: Balance::toJson()");
}

void Balance::fromJson(const nlohmann::json &json) {
    Response::fromJson(json);

    for (const auto &el: m_data.items()) {
        readDecimalValue(el.value(), "adjEq", m_adjEq);
        readDecimalValue(el.value(), "imr", m_imr);
        readDecimalValue(el.value(), "isoEq", m_isoEq);
        readDecimalValue(el.value(), "mgnRatio", m_mgnRatio);
        readDecimalValue(el.value(), "mmr", m_mmr);
        readDecimalValue(el.value(), "notionalUsd", m_notionalUsd);
        readDecimalValue(el.value(), "ordFroz", m_ordFroz);
        readDecimalValue(el.value(), "totalEq", m_totalEq);
        m_uTime = readStringAsInt64(el.value(), "uTime");

        for (const auto &elData: el.value()["details"].items()) {
            BalanceDetail balanceDetail;
            balanceDetail.fromJson(elData.value());
            m_balanceDetails.push_back(balanceDetail);
        }
    }
}

nlohmann::json SystemTime::toJson() const {
    throw std::runtime_error("Unimplemented: SystemTime::toJson()");
}

void SystemTime::fromJson(const nlohmann::json &json) {
    Response::fromJson(json);

    for (const auto &el: m_data.items()) {
        m_ts = readStringAsInt64(el.value(), "ts");
    }
}

nlohmann::json Position::toJson() const {
    throw std::runtime_error("Unimplemented: Position::toJson()");
}

void Position::fromJson(const nlohmann::json &json) {
    m_adl = readStringAsInt(json, "adl");
    readDecimalValue(json, "availPos", m_availPos);
    readDecimalValue(json, "avgPx", m_avgPx);
    m_cTime = readStringAsInt64(json, "cTime");
    readValue<std::string>(json, "ccy", m_ccy);
    readDecimalValue(json, "imr", m_imr);
    readValue<std::string>(json, "instId", m_instId);
    readMagicEnum<InstrumentType>(json, "instType", m_instType);
    readDecimalValue(json, "interest", m_interest);
    readDecimalValue(json, "last", m_last);
    readDecimalValue(json, "lever", m_lever);
    readDecimalValue(json, "liab", m_liab);
    readValue<std::string>(json, "liabCcy", m_liabCcy);
    readDecimalValue(json, "liqPx", m_liqPx);
    readDecimalValue(json, "margin", m_margin);
    readDecimalValue(json, "markPx", m_markPx);
    readMagicEnum<MarginMode>(json, "mgnMode", m_mgnMode);
    readDecimalValue(json, "mgnRatio", m_mgnRatio);
    readDecimalValue(json, "mmr", m_mmr);
    readDecimalValue(json, "notionalUsd", m_notionalUsd);
    readDecimalValue(json, "pos", m_pos);
    readValue<std::string>(json, "posCcy", m_posCcy);
    readValue<std::string>(json, "posId", m_posId);

    std::string posSide;
    readValue<std::string>(json, "posSide", posSide);

    if (const auto posSideVal = OKX::stringToPositionSide(posSide)) {
        m_posSide = *posSideVal;
    }

    readValue<std::string>(json, "tradeId", m_tradeId);

    m_uTime = readStringAsInt64(json, "uTime");
    readDecimalValue(json, "upl", m_upl);
    readDecimalValue(json, "uplRatio", m_uplRatio);
}

nlohmann::json Positions::toJson() const {
    throw std::runtime_error("Unimplemented: Positions::toJson()");
}

void Positions::fromJson(const nlohmann::json &json) {
    Response::fromJson(json);

    for (const auto &el: m_data.items()) {
        Position position;
        position.fromJson(el.value());
        m_positions.push_back(position);
    }
}

nlohmann::json Order::toJson() const {
    nlohmann::json json;
    json["instId"] = m_instId;
    json["tdMode"] = m_tdMode;
    json["clOrdId"] = m_clOrdId;
    json["ccy"] = m_ccy;
    json["side"] = m_side;
    json["posSide"] = magic_enum::enum_name(m_posSide);
    json["ordType"] = m_ordType;
    json["sz"] = m_sz.str();
    json["px"] = m_px.str(3);

    return json;
}

void Order::fromJson(const nlohmann::json &json) {
    throw std::runtime_error("Unimplemented: Order::fromJson()");
}

nlohmann::json OrderResponse::toJson() const {
    throw std::runtime_error("Unimplemented: OrderResponse::toJson()");
}

void OrderResponse::fromJson(const nlohmann::json &json) {
    auto prd = json.dump();

    readValue<std::string>(json, "clOrdId", m_clOrdId);
    readValue<std::string>(json, "ordId", m_ordId);
    readValue<std::string>(json, "tag", m_tag);
    readValue<std::string>(json, "sCode", m_sCode);
    readValue<std::string>(json, "sMsg", m_sMsg);
}

nlohmann::json OrderResponses::toJson() const {
    throw std::runtime_error("Unimplemented: OrderResponses::toJson()");
}

void OrderResponses::fromJson(const nlohmann::json &json) {
    Response::fromJson(json);

    for (const auto &el: m_data.items()) {
        OrderResponse orderResponse;
        orderResponse.fromJson(el.value());
        m_orderResponses.push_back(orderResponse);
    }
}

nlohmann::json OrderDetail::toJson() const {
    throw std::runtime_error("Unimplemented: OrderDetail::toJson()");
}

void OrderDetail::fromJson(const nlohmann::json &json) {
    readMagicEnum<InstrumentType>(json, "instType", m_instType);
    readValue<std::string>(json, "instId", m_instId);
    readValue<std::string>(json, "ccy", m_ccy);
    readValue<std::string>(json, "ordId", m_ordId);
    readValue<std::string>(json, "clOrdId", m_clOrdId);
    readDecimalValue(json, "px", m_px);
    readDecimalValue(json, "sz", m_sz);
    readDecimalValue(json, "pnl", m_pnl);
    readMagicEnum<OrderType>(json, "ordType", m_ordType);
    readMagicEnum<Side>(json, "side", m_side);

    std::string posSide;
    readValue<std::string>(json, "posSide", posSide);

    if (const auto posSideVal = OKX::stringToPositionSide(posSide)) {
        m_posSide = *posSideVal;
    }

    readMagicEnum<MarginMode>(json, "tdMode", m_tdMode);
    readDecimalValue(json, "accFillSz", m_accFillSz);
    readDecimalValue(json, "fillPx", m_fillPx);
    readValue<std::string>(json, "tradeId", m_tradeId);
    readDecimalValue(json, "fillSz", m_fillSz);
    m_fillTime = readStringAsInt64(json, "fillTime");
    readMagicEnum<OrderState>(json, "state", m_state);
    readDecimalValue(json, "avgPx", m_avgPx);
    readDecimalValue(json, "lever", m_lever);
    m_uTime = readStringAsInt64(json, "uTime");
    m_cTime = readStringAsInt64(json, "cTime");
}

nlohmann::json OrderDetails::toJson() const {
    throw std::runtime_error("Unimplemented: OrderDetails::toJson()");
}

void OrderDetails::fromJson(const nlohmann::json &json) {
    Response::fromJson(json);

    for (const auto &el: m_data.items()) {
        OrderDetail orderDetail;
        orderDetail.fromJson(el.value());
        m_orderDetails.push_back(orderDetail);
    }
}
}
