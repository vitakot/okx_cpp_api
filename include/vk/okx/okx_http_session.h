/**
OKX HTTPS Session

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2025 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#ifndef INCLUDE_VK_OKX_HTTP_SESSION_H
#define INCLUDE_VK_OKX_HTTP_SESSION_H

#include <boost/asio/connect.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <string>
#include <map>
#include <nlohmann/json_fwd.hpp>

namespace vk::okx {
namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;

class HTTPSession {
    struct P;
    std::unique_ptr<P> m_p{};

public:
    HTTPSession(const std::string &apiKey, const std::string &apiSecret, const std::string &passphrase);

    ~HTTPSession();

    [[nodiscard]] http::response<http::string_body> get(const std::string &path, const std::map<std::string, std::string> &parameters, bool isPublic = true) const;

    [[nodiscard]] http::response<http::string_body> post(const std::string &path, const nlohmann::json &json, bool isPublic = true) const;

    /**
     * Download binary data from external URL (for ZIP files from static.okx.com)
     * @param url Full URL including https://
     * @return Binary data as vector of bytes
     */
    static std::vector<std::uint8_t> downloadBinary(const std::string &url);
};
} // namespace vk::okx
#endif // INCLUDE_VK_OKX_HTTP_SESSION_H
