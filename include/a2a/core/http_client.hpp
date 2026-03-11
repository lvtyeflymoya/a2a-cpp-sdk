#pragma once

#include <string>
#include <map>
#include <memory>
#include <functional>

namespace a2a {

/**
 * @brief HTTP response
 */
struct HttpResponse {
    int status_code;
    std::string body;
    std::map<std::string, std::string> headers;

    bool is_success() const {
        return status_code >= 200 && status_code < 300;
    }
};

/**
 * @brief HTTP client wrapper (usr libcurl internally)
 */
class HttpClient {
public:
    HttpClient();
    ~HttpClient();

    // Disable copy, enable move. Because impl can not be copied, but can be moved
    HttpClient(const HttpClient&) = delete;
    HttpClient& operator=(const HttpClient&) = delete;
    HttpClient(HttpClient&&) noexcept;    // do not throw exception
    HttpClient& operator=(HttpClient&&) noexcept;

    // Perform GET request
    HttpResponse get(const std::string& url);

    // Perform POST request
    HttpResponse post(const std::string &url,
                    const std::string &body,
                    const std::string &content_type = "application/json");

    /**
     * @brief Perform POST request with stream response
     * @param callback called for each chunk of data received;
     */
    void post_stream(const std::string &url,
                     const std::string &body,
                     const std::string &content_type,
                     std::function <void(const std::string&)> callback);

    /**
     * @brief Set request timeout in seconds
     */
    void set_timeout(long seconds);

    /**
     * @brief Add custom header
     */
    void add_header(const std::string& key, const std::string& value);

    /**
     * @brief Clear all custom headers
     */
    void clear_header();

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};
}   // namespace a2a