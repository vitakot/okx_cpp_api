/**
OKX REST Client

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2025 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include "vk/okx/okx_rest_client.h"
#include "vk/okx/okx_http_session.h"
#include "vk/okx/okx.h"
#include "vk/utils/utils.h"
#include "vk/utils/magic_enum_wrapper.hpp"
#include <mutex>
#include <thread>

namespace vk::okx {
template<typename ValueType>
ValueType handleOKXResponse(const http::response<http::string_body> &response) {
    ValueType retVal;
    retVal.fromJson(nlohmann::json::parse(response.body()));

    if (std::stoi(retVal.code) != 0) {
        throw std::runtime_error(
            fmt::format("OKX API error, code: {}, msg: {}", retVal.code, retVal.msg).c_str());
    }

    return retVal;
}

struct RESTClient::P {
private:
    Instruments m_instruments;
    mutable std::recursive_mutex m_locker;

public:
    RESTClient *parent = nullptr;
    std::shared_ptr<HTTPSession> httpSession;

    explicit P(RESTClient *parent) {
        this->parent = parent;
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
        m_instruments.instruments = instruments;
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
    m_p->httpSession = std::make_shared<HTTPSession>(apiKey, apiSecret, passphrase);
}

RESTClient::~RESTClient() = default;

void
RESTClient::setCredentials(const std::string &apiKey, const std::string &apiSecret,
                           const std::string &passphrase) const {
    m_p->httpSession.reset();
    m_p->httpSession = std::make_shared<HTTPSession>(apiKey, apiSecret, passphrase);
}

std::vector<Ticker> RESTClient::getTickers(const InstrumentType instrumentType) const {
    const std::string path = "/api/v5/market/tickers";
    std::map<std::string, std::string> parameters;

    parameters.insert_or_assign("instType", magic_enum::enum_name(instrumentType));

    const auto response = P::checkResponse(m_p->httpSession->get(path, parameters));
    return handleOKXResponse<Tickers>(response).tickers;
}

std::vector<Instrument> RESTClient::getInstruments(const InstrumentType instrumentType, const bool force) const {
    if (m_p->getInstruments().instruments.empty() || force) {
        const std::string path = "/api/v5/public/instruments";
        std::map<std::string, std::string> parameters;

        parameters.insert_or_assign("instType", magic_enum::enum_name(instrumentType));

        const auto response = P::checkResponse(m_p->httpSession->get(path, parameters));
        m_p->setInstruments(handleOKXResponse<Instruments>(response));
    }

    return m_p->getInstruments().instruments;
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

    const auto response = checkResponse(httpSession->get(path, parameters));
    return handleOKXResponse<Candles>(response).candles;
}

std::vector<Candle>
RESTClient::getHistoricalPrices(const std::string &instId, const BarSize barSize, const std::int64_t from,
                                const std::int64_t to,
                                const std::int32_t limit,
                                const onCandlesDownloaded &writer) const {
    std::vector<Candle> retVal;
    std::vector<Candle> candles;

    if (from < to) {
        candles = m_p->getHistoricalPrices(instId, barSize, from, to, limit);
    }

    while (!candles.empty()) {
        retVal.insert(retVal.end(), candles.begin(), candles.end());
        const std::int64_t lastToTime = candles.back().ts;

        if (writer) {
            if (!candles.back().confirm) {
                candles.pop_back();
            }

            writer(candles);
        }

        candles.clear();

        if (from < lastToTime) {
            candles = m_p->getHistoricalPrices(instId, barSize, from, lastToTime, limit);
        }
    }

    /// Remove last candle if it is not valid
    if (!retVal.empty()) {
        if (!retVal.back().confirm) {
            retVal.pop_back();
        }
    }

    std::ranges::reverse(retVal);
    return retVal;
}

FundingRate RESTClient::getLastFundingRate(const std::string &instId) const {
    const std::string path = "/api/v5/public/funding-rate";
    std::map<std::string, std::string> parameters;
    parameters.insert_or_assign("instId", instId);

    const auto response = P::checkResponse(m_p->httpSession->get(path, parameters));
    return handleOKXResponse<FundingRate>(response);
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

    const auto response = checkResponse(httpSession->get(path, parameters));
    return handleOKXResponse<FundingRates>(response).rates;
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
        const std::int64_t lastToTime = rates.back().fundingTime;
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

    const auto response = P::checkResponse(m_p->httpSession->get(path, parameters, false));
    return handleOKXResponse<Balance>(response);
}

std::int64_t RESTClient::getSystemTime() const {
    const std::string path = "/api/v5/public/time";
    const std::map<std::string, std::string> parameters;

    const auto response = P::checkResponse(m_p->httpSession->get(path, parameters));
    return handleOKXResponse<SystemTime>(response).ts;
}

std::vector<Position> RESTClient::getPositions(const InstrumentType instrumentType, const std::string &instId) const {
    const std::string path = "/api/v5/account/positions";
    std::map<std::string, std::string> parameters;

    parameters.insert_or_assign("instType", magic_enum::enum_name(instrumentType));

    if (!instId.empty()) {
        parameters.insert_or_assign("instId", instId);
    }

    const auto response = P::checkResponse(m_p->httpSession->get(path, parameters, false));
    return handleOKXResponse<Positions>(response).positions;
}

std::vector<OrderResponse>
RESTClient::cancelOrder(const std::string &instId, const std::string &clientOrderId, const std::string &orderId) const {
    const std::string path = "/api/v5/trade/cancel-order";

    nlohmann::json json;
    json["instId"] = instId;
    json["clOrdId"] = clientOrderId;
    json["ordId"] = orderId;

    const auto response = P::checkResponse(m_p->httpSession->post(path, json, false));
    return handleOKXResponse<OrderResponses>(response).orderResponses;
}

std::vector<OrderResponse> RESTClient::placeOrder(const Order &order) const {
    const std::string path = "/api/v5/trade/order";
    const auto response = P::checkResponse(m_p->httpSession->post(path, order.toJson(), false));
    return handleOKXResponse<OrderResponses>(response).orderResponses;
}

std::vector<OrderDetail> RESTClient::getOrderDetail(const std::string &instId, const std::string &clientOrderId,
                                                    const std::string &orderId) const {
    const std::string path = "/api/v5/trade/order";
    std::map<std::string, std::string> parameters;

    parameters.insert_or_assign("instId", instId);
    parameters.insert_or_assign("clOrdId", clientOrderId);
    parameters.insert_or_assign("ordId", orderId);

    const auto response = P::checkResponse(m_p->httpSession->get(path, parameters, false));
    return handleOKXResponse<OrderDetails>(response).orderDetails;
}
}
