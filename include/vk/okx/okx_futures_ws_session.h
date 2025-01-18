/**
OKX Futures WebSocket Session

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2025 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#ifndef INCLUDE_VK_OKX_WS_SESSION_H
#define INCLUDE_VK_OKX_WS_SESSION_H

#include "vk/utils/log_utils.h"
#include <memory>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include "okx_event_models.h"
#include <nlohmann/json_fwd.hpp>

namespace vk::okx::futures {
using onDataEvent = std::function<void(const DataEvent &event)>;

class WebSocketSession final : public std::enable_shared_from_this<WebSocketSession> {
    boost::asio::ip::tcp::resolver m_resolver;
    boost::beast::websocket::stream<boost::beast::ssl_stream<boost::beast::tcp_stream> > m_ws;
    boost::beast::multi_buffer m_buffer;
    std::string m_host;
    std::vector<std::string> m_subscriptions;
    std::string m_subscriptionRequest;
    onLogMessage m_logMessageCB;
    onDataEvent m_dataEventCB;
    boost::asio::steady_timer m_pingTimer;
    std::chrono::time_point<std::chrono::system_clock> m_lastPingTime{};
    std::chrono::time_point<std::chrono::system_clock> m_lastPongTime{};
    mutable std::recursive_mutex m_subscriptionLocker;

    void writeSubscriptionRequest(const std::string &subscriptionRequest);

    std::string readSubscriptionRequest();

    void handleControlEvent(const nlohmann::json &json);

    static bool isControlEvent(const nlohmann::json &json);

    void onResolve(const boost::beast::error_code &ec, const boost::asio::ip::tcp::resolver::results_type &results);

    void onConnect(boost::beast::error_code ec, const boost::asio::ip::tcp::resolver::results_type::endpoint_type &ep);

    void onSSLHandshake(const boost::beast::error_code &ec);

    void onHandshake(const boost::beast::error_code &ec);

    void onRead(const boost::beast::error_code &ec, std::size_t bytes_transferred);

    void onWrite(const boost::beast::error_code &ec, std::size_t bytes_transferred);

    void onClose(const boost::beast::error_code &ec);

    void ping();

    void onPingTimer(const boost::beast::error_code &ec);

public:
    explicit WebSocketSession(boost::asio::io_context &ioc, boost::asio::ssl::context &ctx,
                              const onLogMessage &onLogMessageCB);

    ~WebSocketSession();

    /**
     * Run the session.
     * @param host
     * @param port
     * @param subscriptionRequest Must not be empty
     * @param dataEventCB Data Message callback
     */
    void
    run(const std::string &host, const std::string &port, const std::string &subscriptionRequest,
        const onDataEvent &dataEventCB);

    /**
     * Close the session asynchronously
     */
    void close();

    /**
     * Subscribe WebSocket according to the subscriptionFilter
     * @param subscriptionRequest
     */
    void subscribe(const std::string &subscriptionRequest);

    /**
     * Check if a stream is already subscribed
     * @param subscriptionRequest
     * @return True if subscribed
     */
    [[nodiscard]] bool isSubscribed(const std::string &subscriptionRequest) const;
};
}
#endif //INCLUDE_VK_OKX_WS_SESSION_H
