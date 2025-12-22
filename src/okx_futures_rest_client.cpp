/**
OKX Futures REST Client

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2025 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include "vk/okx/okx_futures_rest_client.h"
#include "vk/okx/okx_http_session.h"
#include "vk/okx/okx.h"
#include "vk/utils/utils.h"
#include "vk/utils/magic_enum_wrapper.hpp"
#include <mutex>
#include <thread>

namespace vk::okx::futures {
template<typename ValueType>
ValueType handleOKXResponse(const http::response<http::string_body> &response) {
    ValueType retVal;
    retVal.fromJson(nlohmann::json::parse(response.body()));

    if (std::stoi(retVal.m_code) != 0) {
        throw std::runtime_error(
            fmt::format("OKX API error, code: {}, msg: {}", retVal.m_code, retVal.m_msg).c_str());
    }

    return retVal;
}

struct RESTClient::P {
private:
    Instruments m_instruments;
    mutable std::recursive_mutex m_locker;

public:
    RESTClient *m_parent = nullptr;
    std::shared_ptr<HTTPSession> m_httpSession;

    explicit P(RESTClient *parent) {
        m_parent = parent;
    }

    [[nodiscard]] Instruments getInstruments() const {
        std::lock_guard lk(m_locker);
        return m_instruments;
    }

    void setInstruments(const Instruments &instruments) {
        std::lock_guard lk(m_locker);
        m_instruments = instruments;
    }

    void setInstruments(const std::vector<Instrument> &instruments) {
        std::lock_guard lk(m_locker);
        m_instruments.m_instruments = instruments;
    }

    static http::response<http::string_body> checkResponse(const http::response<http::string_body> &response) {
        if (response.result() != http::status::ok) {
            throw std::runtime_error(
                fmt::format("Bad response, code {}, msg: {}", response.result_int(), response.body()).c_str());
        }
        return response;
    }

    std::vector<Candle>
    getHistoricalPrices(const std::string &instId, BarSize barSize, std::int64_t from, std::int64_t to,
                        std::int32_t limit) const;

    std::vector<FundingRate>
    getFundingRates(const std::string &instId, int64_t from, int64_t to, int limit) const;
};

RESTClient::RESTClient(const std::string &apiKey, const std::string &apiSecret, const std::string &passphrase) : m_p(
    std::make_unique<P>(this)) {
    m_p->m_httpSession = std::make_shared<HTTPSession>(apiKey, apiSecret, passphrase);
}

RESTClient::~RESTClient() = default;

void
RESTClient::setCredentials(const std::string &apiKey, const std::string &apiSecret,
                           const std::string &passphrase) const {
    m_p->m_httpSession.reset();
    m_p->m_httpSession = std::make_shared<HTTPSession>(apiKey, apiSecret, passphrase);
}

std::vector<Ticker> RESTClient::getTickers(const InstrumentType instrumentType) const {
    const std::string path = "/api/v5/market/tickers";
    std::map<std::string, std::string> parameters;

    parameters.insert_or_assign("instType", magic_enum::enum_name(instrumentType));

    const auto response = P::checkResponse(m_p->m_httpSession->get(path, parameters));
    return handleOKXResponse<Tickers>(response).m_tickers;
}

std::vector<Instrument> RESTClient::getInstruments(const InstrumentType instrumentType, const bool force) const {
    if (m_p->getInstruments().m_instruments.empty() || force) {
        const std::string path = "/api/v5/public/instruments";
        std::map<std::string, std::string> parameters;

        parameters.insert_or_assign("instType", magic_enum::enum_name(instrumentType));

        const auto response = P::checkResponse(m_p->m_httpSession->get(path, parameters));
        m_p->setInstruments(handleOKXResponse<Instruments>(response));
    }

    return m_p->getInstruments().m_instruments;
}

void RESTClient::setInstruments(const std::vector<Instrument> &instruments) const {
    m_p->setInstruments(instruments);
}

std::vector<Candle>
RESTClient::P::getHistoricalPrices(const std::string &instId, const BarSize barSize, const std::int64_t from,
                                   const std::int64_t to,
                                   const std::int32_t limit) const {
    const std::string path = "/api/v5/market/history-candles";
    std::map<std::string, std::string> parameters;

    parameters.insert_or_assign("instId", instId);
    parameters.insert_or_assign("bar", magic_enum::enum_name(barSize));

    if (from != -1) {
        parameters.insert_or_assign("after", std::to_string(to));
    }

    if (to != -1) {
        parameters.insert_or_assign("before", std::to_string(from));
    }

    if (limit != -1) {
        parameters.insert_or_assign("limit", std::to_string(limit));
    }

    const auto response = checkResponse(m_httpSession->get(path, parameters));
    return handleOKXResponse<Candles>(response).m_candles;
}

std::vector<Candle>
RESTClient::getHistoricalPrices(const std::string &instId, const BarSize barSize, const std::int64_t from,
                                const std::int64_t to,
                                const std::int32_t limit) const {
    std::vector<Candle> retVal;
    std::vector<Candle> candles;

    if (from < to) {
        candles = m_p->getHistoricalPrices(instId, barSize, from, to, limit);
    }

    while (!candles.empty()) {
        retVal.insert(retVal.end(), candles.begin(), candles.end());
        const std::int64_t lastToTime = candles.back().m_ts;
        candles.clear();

        if (from < lastToTime) {
            candles = m_p->getHistoricalPrices(instId, barSize, from, lastToTime, limit);
        }
    }

    /// Remove last candle if it is not valid
    if (!retVal.empty()) {
        if (!retVal.back().m_confirm) {
            retVal.pop_back();
        }
    }

    std::ranges::reverse(retVal);
    return retVal;
}

FundingRate RESTClient::getLastFundingRate(const std::string &instId) const {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(35ms);
    // const std::string path = "/api/v5/public/funding-rate";
    // std::map<std::string, std::string> parameters;
    //
    // parameters.insert_or_assign("instId", instId);
    //
    // const auto response = P::checkResponse(m_p->m_httpSession->get(path, parameters));
    // return handleOKXResponse<FundingRate>(response);
    return {};
}

std::vector<FundingRate>
RESTClient::P::getFundingRates(const std::string &instId, const int64_t from, const int64_t to, const int limit) const {
    const std::string path = "/api/v5/public/funding-rate-history";
    std::map<std::string, std::string> parameters;

    parameters.insert_or_assign("instId", instId);

    if (from != -1) {
        parameters.insert_or_assign("after", std::to_string(to));
    }

    if (to != -1) {
        parameters.insert_or_assign("before", std::to_string(from));
    }

    if (limit != -1) {
        parameters.insert_or_assign("limit", std::to_string(limit));
    }

    const auto response = checkResponse(m_httpSession->get(path, parameters));
    return handleOKXResponse<FundingRates>(response).m_rates;
}

std::vector<FundingRate>
RESTClient::getFundingRates(const std::string &instId, const int64_t from, const int64_t to, const int limit) const {
    std::vector<FundingRate> retVal;
    std::vector<FundingRate> rates;

    if (from < to) {
        rates = m_p->getFundingRates(instId, from, to, limit);
    }

    while (!rates.empty()) {
        retVal.insert(retVal.end(), rates.begin(), rates.end());
        const std::int64_t lastToTime = rates.back().m_fundingTime;
        rates.clear();

        if (from < lastToTime) {
            rates = m_p->getFundingRates(instId, from, lastToTime, limit);
        }
    }

    std::ranges::reverse(retVal);
    return retVal;
}

Balance RESTClient::getBalance(const std::string &ccy) const {
    const std::string path = "/api/v5/account/balance";
    std::map<std::string, std::string> parameters;

    if (!ccy.empty()) {
        parameters.insert_or_assign("ccy", ccy);
    }

    const auto response = P::checkResponse(m_p->m_httpSession->get(path, parameters, false));
    return handleOKXResponse<Balance>(response);
}

std::int64_t RESTClient::getSystemTime() const {
    const std::string path = "/api/v5/public/time";
    const std::map<std::string, std::string> parameters;

    const auto response = P::checkResponse(m_p->m_httpSession->get(path, parameters));
    return handleOKXResponse<SystemTime>(response).m_ts;
}

std::vector<Position> RESTClient::getPositions(const InstrumentType instrumentType, const std::string &instId) const {
    const std::string path = "/api/v5/account/positions";
    std::map<std::string, std::string> parameters;

    parameters.insert_or_assign("instType", magic_enum::enum_name(instrumentType));

    if (!instId.empty()) {
        parameters.insert_or_assign("instId", instId);
    }

    const auto response = P::checkResponse(m_p->m_httpSession->get(path, parameters, false));
    return handleOKXResponse<Positions>(response).m_positions;
}

std::vector<OrderResponse>
RESTClient::cancelOrder(const std::string &instId, const std::string &clientOrderId, const std::string &orderId) const {
    const std::string path = "/api/v5/trade/cancel-order";

    nlohmann::json json;
    json["instId"] = instId;
    json["clOrdId"] = clientOrderId;
    json["ordId"] = orderId;

    const auto response = P::checkResponse(m_p->m_httpSession->post(path, json, false));
    return handleOKXResponse<OrderResponses>(response).m_orderResponses;
}

std::vector<OrderResponse> RESTClient::placeOrder(const Order &order) const {
    const std::string path = "/api/v5/trade/order";
    const auto response = P::checkResponse(m_p->m_httpSession->post(path, order.toJson(), false));
    return handleOKXResponse<OrderResponses>(response).m_orderResponses;
}

std::vector<OrderDetail> RESTClient::getOrderDetail(const std::string &instId, const std::string &clientOrderId,
                                                    const std::string &orderId) const {
    const std::string path = "/api/v5/trade/order";
    std::map<std::string, std::string> parameters;

    parameters.insert_or_assign("instId", instId);
    parameters.insert_or_assign("clOrdId", clientOrderId);
    parameters.insert_or_assign("ordId", orderId);

    const auto response = P::checkResponse(m_p->m_httpSession->get(path, parameters, false));
    return handleOKXResponse<OrderDetails>(response).m_orderDetails;
}
}
