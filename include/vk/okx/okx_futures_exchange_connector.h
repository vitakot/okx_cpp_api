/**
OKX Exchange Connector

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2022 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/


#ifndef INCLUDE_VK_MEXC_EXCHANGE_CONNECTOR_H
#define INCLUDE_VK_MEXC_EXCHANGE_CONNECTOR_H

#include "vk/interface/i_exchange_connector.h"
#include "vk/common/module_factory.h"
#include <memory>

#ifdef _WIN32
#include "magic_enum/magic_enum.hpp"
#else
#include "magic_enum.hpp"
#endif

namespace vk {
class OKXFuturesExchangeConnector final : public IExchangeConnector {
    struct P;
    std::unique_ptr<P> m_p{};

public:
    OKXFuturesExchangeConnector();

    ~OKXFuturesExchangeConnector() override;

    [[nodiscard]] std::string exchangeId() const override;

    [[nodiscard]] std::string version() const override;

    void setLoggerCallback(const onLogMessage &onLogMessageCB) override;

    void login(const std::tuple<std::string, std::string, std::string> &credentials) override;

    Trade placeOrder(const Order &order) override;

    [[nodiscard]] TickerPrice getTickerPrice(const std::string &symbol) const override;

    [[nodiscard]] Balance getAccountBalance(const std::string &currency) const override;

    [[nodiscard]] FundingRate getFundingRate(const std::string &symbol) const override;

    [[nodiscard]] std::vector<FundingRate> getFundingRates() const override;

    [[nodiscard]] std::vector<Ticker> getTickerInfo(const std::string& symbol) const override;

    [[nodiscard]] std::int64_t getServerTime() const override;

    static std::shared_ptr<IExchangeConnector> createInstance() {
        return std::make_shared<OKXFuturesExchangeConnector>();
    }
};

BOOST_SYMBOL_EXPORT IModuleFactory *getModuleFactory() {
    if (!g_moduleFactory) {
        FactoryInfo factoryInfo;
        factoryInfo.m_id = std::string(magic_enum::enum_name(ExchangeId::OKXFutures));
        factoryInfo.m_description = "OKX CEX - Futures";

        g_moduleFactory = new ModuleFactory(factoryInfo);
        g_moduleFactory->registerClassByName<IExchangeConnector>(
            std::string(magic_enum::enum_name(ExchangeId::OKXFutures)),
            &OKXFuturesExchangeConnector::createInstance);
    } else {
        return nullptr;
    }

    return g_moduleFactory;
}
}
#endif //INCLUDE_VK_MEXC_EXCHANGE_CONNECTOR_H
