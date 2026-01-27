/**
OKX HTTPS Session

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2025 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include "vk/okx/okx_http_session.h"
#include "vk/utils/utils.h"
#include "nlohmann/json.hpp"
#include <boost/asio/ssl.hpp>
#include <boost/beast/version.hpp>
#include "base64.h"
#include "date.h"
#include <openssl/hmac.h>

namespace vk::okx {
namespace ssl = boost::asio::ssl;
using tcp = net::ip::tcp;

constexpr auto API_MAINNET_URI = "www.okx.com";

struct HTTPSession::P {
    net::io_context ioc;
    std::string apiKey;
    std::string apiSecret;
    std::string passphrase;
    std::string uri;
    const EVP_MD *evpMd;

    P() : evpMd(EVP_sha256()) {
    }

    http::response<http::string_body> request(http::request<http::string_body> req);

    static std::string createQueryStr(const std::map<std::string, std::string> &parameters) {
        std::string queryStr;

        for (const auto &[fst, snd]: parameters) {
            queryStr.append(fst);
            queryStr.append("=");
            queryStr.append(snd);
            queryStr.append("&");
        }

        if (!queryStr.empty()) {
            queryStr.pop_back();
        }
        return queryStr;
    }

    void authenticatePost(http::request<http::string_body> &req, const nlohmann::json &json) const {
        const auto bodyString = json.dump();

        std::string parameterString;

        const auto now = time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
        const auto ts = date::format("%FT%T", date::sys_time{now}).append("Z");

        parameterString.append(ts);
        parameterString.append("POST");
        parameterString.append(req.target());
        parameterString.append(bodyString);

        unsigned char digest[SHA256_DIGEST_LENGTH];
        unsigned int digestLength = SHA256_DIGEST_LENGTH;

        HMAC(evpMd, apiSecret.data(), static_cast<int>(apiSecret.size()),
             reinterpret_cast<const unsigned char *>(parameterString.data()),
             parameterString.length(), digest, &digestLength);

        const std::string signature = base64_encode(digest, sizeof(digest));

        req.body() = bodyString;
        req.prepare_payload();

        req.set("OK-ACCESS-KEY", apiKey);
        req.set("OK-ACCESS-SIGN", signature);
        req.set("OK-ACCESS-TIMESTAMP", ts);
        req.set("OK-ACCESS-PASSPHRASE", passphrase);
        req.set(http::field::content_type, "application/json");
    }

    void authenticateGet(http::request<http::string_body> &req) const {
        std::string parameterString;

        const auto now = time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
        const auto ts = date::format("%FT%T", date::sys_time{now}).append("Z");

        parameterString.append(ts);
        parameterString.append("GET");
        parameterString.append(req.target());

        unsigned char digest[SHA256_DIGEST_LENGTH];
        unsigned int digestLength = SHA256_DIGEST_LENGTH;

        HMAC(evpMd, apiSecret.data(), static_cast<int>(apiSecret.size()),
             reinterpret_cast<const unsigned char *>(parameterString.data()),
             parameterString.length(), digest, &digestLength);

        const std::string signature = base64_encode(digest, sizeof(digest));

        req.set("OK-ACCESS-KEY", apiKey);
        req.set("OK-ACCESS-SIGN", signature);
        req.set("OK-ACCESS-TIMESTAMP", ts);
        req.set("OK-ACCESS-PASSPHRASE", passphrase);
    }
};

HTTPSession::HTTPSession(const std::string &apiKey, const std::string &apiSecret, const std::string &passphrase) : m_p(
    std::make_unique<P>()) {
    m_p->uri = API_MAINNET_URI;
    m_p->apiKey = apiKey;
    m_p->apiSecret = apiSecret;
    m_p->passphrase = passphrase;
}

http::response<http::string_body>
HTTPSession::get(const std::string &path, const std::map<std::string, std::string> &parameters,
                 const bool isPublic) const {
    std::string finalPath = path;

    if (const auto queryString = P::createQueryStr(parameters); !queryString.empty()) {
        finalPath.append("?");
        finalPath.append(queryString);
    }

    http::request<http::string_body> req{http::verb::get, finalPath, 11};

    if (!isPublic) {
        m_p->authenticateGet(req);
    }

    return m_p->request(req);
}

http::response<http::string_body>
HTTPSession::post(const std::string &path, const nlohmann::json &json, const bool isPublic) const {
    http::request<http::string_body> req{http::verb::post, path, 11};

    if (!isPublic) {
        m_p->authenticatePost(req, json);
    }

    return m_p->request(req);
}

HTTPSession::~HTTPSession() = default;

http::response<http::string_body> HTTPSession::P::request(
    http::request<http::string_body> req) {
    req.set(http::field::host, uri);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    ssl::context ctx{ssl::context::sslv23_client};
    ctx.set_default_verify_paths();

    tcp::resolver resolver{ioc};
    ssl::stream<tcp::socket> stream{ioc, ctx};

    // Set SNI Hostname (many hosts need this to handshake successfully)
    if (!SSL_set_tlsext_host_name(stream.native_handle(), uri.c_str())) {
        boost::system::error_code ec{
            static_cast<int>(ERR_get_error()),
            net::error::get_ssl_category()
        };
        throw boost::system::system_error{ec};
    }

    auto const results = resolver.resolve(uri, "443");
    net::connect(stream.next_layer(), results.begin(), results.end());
    stream.handshake(ssl::stream_base::client);

    http::write(stream, req);
    beast::flat_buffer buffer;
    http::response<http::string_body> response;
    http::read(stream, buffer, response);

    boost::system::error_code ec;

    [[maybe_unused]] auto rc = stream.shutdown(ec);
    if (ec == boost::asio::error::eof) {
        // Rationale:
        // http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
        ec.assign(0, ec.category());
    }

    return response;
}
}
