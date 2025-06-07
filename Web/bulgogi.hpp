/// Copyright (c) 2025 bulgogi-framework
/// SPDX-License-Identifier: MIT

#pragma once

#include <boost/beast/http.hpp>
#include <boost/json.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <regex>
#include <jh/pod>
#include "marcos.hpp"


namespace bulgogi {
    namespace beast = boost::beast;
    namespace http = beast::http;
    using Request = http::request<http::string_body>;
    using Response = http::response<http::string_body>;
}

namespace bulgogi::cors {
    [[maybe_unused]] inline constexpr std::string_view none = "null";
    [[maybe_unused]] inline constexpr std::string_view all  = "*";
}

namespace bulgogi::ipv4 {

    namespace detail {
        inline bool parse_ipv4_strict(const std::string& ip, uint8_t& a, uint8_t& b, uint8_t& c, uint8_t& d) {
            size_t pos = 0, last = 0;
            unsigned int values[4] = {0};

            for (int i = 0; i < 4; ++i) {
                pos = ip.find('.', last);
                std::string token = (i < 3) ? ip.substr(last, pos - last) : ip.substr(last);
                char* end;
                auto val = std::strtoul(token.c_str(), &end, 10);
                if (*end != '\0' || val > 255) return false;
                values[i] = val;
                last = pos + 1;
            }

            a = static_cast<uint8_t>(values[0]);
            b = static_cast<uint8_t>(values[1]);
            c = static_cast<uint8_t>(values[2]);
            d = static_cast<uint8_t>(values[3]);
            return true;
        }
    }

    inline bool is_self(const std::string &ip) {
        return ip == "127.0.0.1" || ip == "0.0.0.0";
    }

    inline bool is_ipv6(const std::string &ip) {
        return (ip.find(':') != std::string::npos);
    }

    inline bool is_private_lan_ip(const std::string &ip) {
        if (is_ipv6(ip)) return false;
        uint8_t a, b, c, d;
        if (!detail::parse_ipv4_strict(ip, a, b, c, d)) return false;

        if (a == 10) return true;                          // 10.0.0.0/8
        if (a == 192 && b == 168) return true;             // 192.168.0.0/16
        if (a == 172 && b >= 16 && b <= 31) return true;   // 172.16.0.0/12

        return false;
    }

    /**
     * @brief Check if the given IP belongs to the local/private LAN.
     * IPv6 is explicitly excluded. Only IPv4 loop-back or private subnets are accepted.
     */
    inline bool is_internal_network(const std::string &ip) {
        return is_self(ip) || is_private_lan_ip(ip);
    }
}


namespace views {
    /// @brief Check if the request method matches the expected method
    void check_head(const bulgogi::Request &req);
}

namespace bulgogi {

    /**
     * @brief Check if the request has a JSON Content-Type.
     * @param req Incoming HTTP request.
     * @return true if Content-Type starts with "application/json".
     */
    [[maybe_unused]] inline bool is_json(const Request &req) {
        return req[http::field::content_type].starts_with("application/json");
    }

    /**
     * @brief Set response as JSON body with status code.
     * @param res Response to populate.
     * @param value JSON value to serialize into body.
     * @param status_code HTTP status code (default: 200).
     */
    inline void set_json(Response &res, const boost::json::value &value, int status_code = 200) {
        res.result(http::status(status_code));
        res.set(http::field::content_type, "application/json");
        res.body() = boost::json::serialize(value);
        res.prepare_payload();
    }

    /**
     * @brief Set response as plain text body.
     * @param res Response to populate.
     * @param text Body content.
     * @param status_code HTTP status code (default: 200).
     */
    inline void set_text(Response &res, std::string_view text, int status_code = 200) {
        res.result(http::status(status_code));
        res.set(http::field::content_type, "text/plain");
        res.body() = std::string(text);
        res.prepare_payload();
    }

    /**
     * @brief Set response as HTML content.
     * @param res Response to populate.
     * @param html HTML string.
     * @param status_code HTTP status code (default: 200).
     */
    [[maybe_unused]] inline void set_html(Response &res, std::string_view html, int status_code = 200) {
        res.result(http::status(status_code));
        res.set(http::field::content_type, "text/html");
        res.body() = std::string(html);
        res.prepare_payload();
    }

    /**
     * @brief Set response as binary file download (octet-stream).
     * @param res Response to populate.
     * @param binary_data File content.
     * @param filename Download file name (Content-Disposition header).
     */
    [[maybe_unused]] inline void set_binary(Response &res, std::string_view binary_data, const std::string &filename) {
        res.result(http::status::ok);
        res.set(http::field::content_type, "application/octet-stream");
        res.set(http::field::content_disposition, "attachment; filename=\"" + filename + "\"");
        res.body() = std::string(binary_data);
        res.prepare_payload();
    }

    /**
     * @tparam Mime MIME type tag (e.g., "csv", "yaml") as jh::pod::array.
     * @brief Set response as downloadable text file with specific MIME type.
     * @param res Response object.
     * @param content File content.
     * @param filename Suggested filename for download.
     * @example
     * @code{.cpp}
     *
     * namespace download_types {
     *     constexpr jh::pod::array<char, 32> CSV  = {"csv"};
     *     constexpr jh::pod::array<char, 32> TSV  = {"tab-separated-values"};
     *     constexpr jh::pod::array<char, 32> YAML = {"yaml"};
     *     constexpr jh::pod::array<char, 32> HTML = {"html"};
     *     constexpr jh::pod::array<char, 32> PLAIN = {"plain"};
     *     constexpr jh::pod::array<char, 32> MD   = {"markdown"};
     *     constexpr jh::pod::array<char, 32> XML  = {"xml"};
     * }
     * 
     * using download_csv = bulgogi::set_download<download_types::CSV>;
     * using download_tsv = bulgogi::set_download<download_types::TSV>;
     * using download_yaml = bulgogi::set_download<download_types::YAML>;
     * using download_html = bulgogi::set_download<download_types::HTML>;
     * using download_plain = bulgogi::set_download<download_types::PLAIN>;
     * using download_md = bulgogi::set_download<download_types::MD>;
     * using download_xml = bulgogi::set_download<download_types::XML>;
     * @endcode
     */
    template<jh::pod::array<char, 32> Mime>
    struct [[maybe_unused]] set_download {
        [[maybe_unused]]
        static void apply(Response &res, std::string_view content, const std::string &filename) { // NOLINT
            res.result(http::status::ok);
            res.set(http::field::content_type, "text/" + std::string(Mime.data));
            res.set(http::field::content_disposition, "attachment; filename=\"" + filename + "\"");
            res.body() = std::string(content);
            res.prepare_payload();
        }
    };

    /**
     * @brief Parse and return JSON object from request body.
     * @param req Request with JSON body.
     * @return Parsed boost::json::object.
     * @throws boost::json::system_error on invalid JSON.
     */
    [[maybe_unused]] inline boost::json::object get_json_obj(const Request &req) {
        return boost::json::parse(req.body()).as_object();
    }

    /**
     * @brief Convert HTTP verb to string representation.
     * @param method HTTP verb.
     * @return String view of the method name (e.g., "GET").
     */
    [[maybe_unused]] inline std::string_view method_string(http::verb method) {
        return http::to_string(method);
    }

    /**
     * @brief Issue an HTTP redirect with given location and status code.
     * @param res Response object.
     * @param location Target URL.
     * @param code Redirect status code (default: 302).
     */
    [[maybe_unused]] inline void set_redirect(Response &res, const std::string &location, int code = 302) {
        res.result(static_cast<http::status>(code));
        res.set(http::field::location, location);
        res.set(http::field::content_type, "text/plain");
        res.body() = "Redirecting to " + location;
        res.prepare_payload();
    }

    /**
     * @brief Set CORS headers for response.
     * @param res Response to modify.
     * @param allow_origin Allowed origin (default: "*").
     * @param allowed_methods Allowed HTTP methods for CORS.
     * @param credentials Boolean, true if credentials is allowed, false otherwise
     */
    inline void apply_cors([[maybe_unused]] bulgogi::Response &res,
                           [[maybe_unused]] std::string_view allow_origin = "*",
                           [[maybe_unused]] std::initializer_list<http::verb> allowed_methods = {},
                           bool credentials = false) {

#ifndef NO_CORS
        if (allow_origin.empty() || allow_origin == "null") {
            // nullptr or "null"
            return; // CORS disabled for this route
        }

        res.set(http::field::access_control_allow_origin, allow_origin);

        if (allowed_methods.size()) {
            std::ostringstream oss;
            for (auto it = allowed_methods.begin(); it != allowed_methods.end(); ++it) {
                oss << http::to_string(*it);
                if (std::next(it) != allowed_methods.end()) oss << ", ";
            }
            oss << ", " << http::to_string(http::verb::options);  // preflight
            res.set(http::field::access_control_allow_methods, oss.str());
        }

        res.set(http::field::access_control_allow_headers, "Content-Type, Authorization");
        res.set(http::field::access_control_max_age, STR(CORS_MAX_AGE));

        if (credentials) {
            res.set(http::field::access_control_allow_credentials, "true");
        }
#endif
    }

    /**
     * @brief Check if the incoming HTTP request uses an allowed method.
     *
     * This helper:
     * - Validates the HTTP verb against an expected list.
     * - Automatically handles `OPTIONS` preflight requests with correct CORS headers.
     * - Sets a `405` JSON error response if the method is not allowed.
     *
     * You should call this **immediately after entering a handler**, like so:
     * @code
     * if (!check_method(req, http::verb::post, res)) return;
     * @endcode
     *
     * @param req The incoming request object.
     * @param allowed_methods One or more allowed HTTP verbs.
     * @param res The response to populate on error or preflight.
     * @param allow_origin Optional CORS origin override (defaults to "*").
     * @param credentials Boolean, true if credentials is allowed, false otherwise
     * @return `true` if the method is allowed and the handler should proceed.
     *         `false` if preflight was handled or method was denied.
     */
    inline bool check_method(const Request &req,
                             std::initializer_list<http::verb> allowed_methods,
                             Response &res,
                             std::string_view allow_origin = "*",
                             bool credentials = false) {
#if defined(NO_CORS)
        constexpr bool cors_disabled = true;
#else
        const bool cors_disabled = (
                allow_origin.empty() ||
                allow_origin == "null"
        );
#endif
        const auto origin_hdr = req.find(http::field::origin);
        const auto req_method = req.method();
        if (cors_disabled) {

            // CORS request will have origin

            if (origin_hdr != req.end()) {
                set_json(res, {
                        {"error", "CORS disabled"},
                        {"detail", "This endpoint does not allow cross-origin access"}
                }, 403);
                return false;
            }

            // Reject Preflight
            if (req_method == http::verb::options) {
                set_json(res, {
                        {"error", "Preflight denied"},
                        {"detail", "CORS preflight not allowed on this route"}
                }, 405);
                return false;
            }
        }

#ifndef NO_CORS
        // Validate credentials + origin
        if (credentials && (allow_origin == "*" || allow_origin.empty() || allow_origin == "null")) {
            set_json(res, {
                    {"error", "CORS misconfiguration: credentials=true requires specific origin, not '*'"}
            }, 500);
            return false;
        }

        if (origin_hdr != req.end()) {
            const std::string_view req_origin = origin_hdr->value();

            if (allow_origin != "*" && allow_origin != req_origin) {
                set_json(res, {
                        {"error",   "CORS origin mismatch"},
                        {"allowed", std::string(allow_origin)},
                        {"got",     std::string(req_origin)}
                }, 403);
                return false;
            }
        }
#endif
        // CORS Allowed

        if (req_method == http::verb::options) {
            // OPTIONS preflight
            apply_cors(res, allow_origin, allowed_methods, credentials);  // get cors header automatically
            return false; // exist early
        }

        const bool allowed = std::any_of(allowed_methods.begin(), allowed_methods.end(),
                                         [&](http::verb v) { return v == req_method; });

        if (!allowed) {
            set_json(res, {
                    {"error",    "Method Not Allowed"},
                    {"expected", [allowed_methods] {
                        std::ostringstream oss;
                        for (auto it = allowed_methods.begin(); it != allowed_methods.end(); ++it) {
                            oss << http::to_string(*it);
                            if (std::next(it) != allowed_methods.end()) oss << ", ";
                        }
                        return oss.str();
                    }()},
                    {"got",      http::to_string(req_method)}
            }, 405);

            apply_cors(res, allow_origin, allowed_methods, credentials);

            return false;
        }

        apply_cors(res, allow_origin, allowed_methods, credentials);  // get cors header automatically

        return true;
    }


    /**
     * @brief Shorthand for checking a single allowed HTTP method.
     * @param req Request object.
     * @param allowed_method Single accepted HTTP verb.
     * @param res Response for error handling.
     * @param allow_origin CORS origin (default "*").
     * @param credentials Boolean, true if credentials is allowed, false otherwise
     * @return true if method is allowed; false otherwise.
     */
    inline bool check_method(const Request &req,
                             http::verb allowed_method,
                             Response &res,
                             std::string_view allow_origin = "*",
                             bool credentials = false) {
        return check_method(req, {allowed_method}, res, allow_origin, credentials);
    }

    /**
     * @brief Extract query string parameter from URL.
     * @param req HTTP request with URL.
     * @param key Name of the query parameter.
     * @return Value if key exists; std::nullopt otherwise.
     */
    [[maybe_unused]] inline std::optional<std::string> get_query_param(
            const boost::beast::http::request<boost::beast::http::string_body> &req,
            std::string_view key) {
        const std::string target = std::string(req.target());
        const std::string_view target_view = target;
        std::string_view query = target_view.substr(target.find('?') + 1); // query = view from owned string

        while (!query.empty()) {
            auto eq = query.find('=');
            if (eq == std::string_view::npos) break;

            std::string_view k = query.substr(0, eq);
            query.remove_prefix(eq + 1);

            auto amp = query.find('&');
            std::string_view v = (amp == std::string_view::npos) ? query : query.substr(0, amp);
            if (amp != std::string_view::npos) query.remove_prefix(amp + 1);
            else query = {};

            if (k == key) {
                return std::string(v); // NOLINT(clang-analyzer-cplusplus.NewDeleteLeaks)
                // safe: v from target_view, local variable
            }
        }
        return std::nullopt;
    }

}
