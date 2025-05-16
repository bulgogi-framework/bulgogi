#include "views.hpp"
#include "bulgogi.hpp"
#include "template.hpp"
#include <boost/asio/ip/tcp.hpp>
#include <boost/json.hpp>
#include <iostream>

namespace json = boost::json;
using bulgogi::Request; /// @brief HTTP request
using bulgogi::Response; /// @brief HTTP response
using bulgogi::check_method; /// @brief Check HTTP method
using bulgogi::set_json; /// @brief Set JSON response

/// @brief Methods to download text files, un-comment to use
/*
namespace download_types {
    constexpr jh::pod::array<char, 32> CSV  = {"csv"};
    constexpr jh::pod::array<char, 32> TSV  = {"tab-separated-values"};
    constexpr jh::pod::array<char, 32> YAML = {"yaml"};
    constexpr jh::pod::array<char, 32> HTML = {"html"};
    constexpr jh::pod::array<char, 32> PLAIN = {"plain"};
    constexpr jh::pod::array<char, 32> MD   = {"markdown"};
    constexpr jh::pod::array<char, 32> XML  = {"xml"};
}

using download_csv = bulgogi::set_download<download_types::CSV>;
using download_tsv = bulgogi::set_download<download_types::TSV>;
using download_yaml = bulgogi::set_download<download_types::YAML>;
using download_html = bulgogi::set_download<download_types::HTML>;
using download_plain = bulgogi::set_download<download_types::PLAIN>;
using download_md = bulgogi::set_download<download_types::MD>;
using download_xml = bulgogi::set_download<download_types::XML>;
*/

/// @brief Global function map for registered urls
std::unordered_map<std::string, views::HandlerFunc> views::function_map;

/// @brief Atomic boolean to signal server shutdown
extern std::atomic<bool> g_should_exit;

/// @brief Global acceptor for TCP connections
extern std::unique_ptr<boost::asio::ip::tcp::acceptor> global_acceptor;

#ifndef NDEBUG
/// @brief Default root view for the server
REGISTER_ROOT_VIEW(default_root) {
    if (!check_method(req, bulgogi::http::verb::get, res)) return;
    bulgogi::set_html(res, default_page::html, 200);
}

#endif


void views::init() {
/// Todo: Add initialization code if needed
}


void views::atexit() {
/// Todo: Add cleanup code if needed
}


REGISTER_VIEW(ping) {
    if (!check_method(req, bulgogi::http::verb::get, res)) return;
    set_json(res, {{"status", "alive"}});
}

REGISTER_VIEW(shutdown_server) {
    if (!check_method(req, bulgogi::http::verb::post, res)) return;

    if (!g_should_exit.exchange(true)) {
        std::cout << "Called Exit\n";

        if (global_acceptor && global_acceptor->is_open()) {
            boost::system::error_code ec;
            global_acceptor->cancel(ec); // NOLINT
        }

        try {
            boost::asio::io_context ioc;
            boost::asio::ip::tcp::socket s(ioc);
            s.connect({boost::asio::ip::address_v4::loopback(), PORT});
        } catch (...) {}
    }

    set_json(res, {{"status", "server_shutdown_requested"}});
}

/// @brief Following functions are examples of how to handle GET and POST requests, un-comment to use
/*
// === Example GET ===
REGISTER_VIEW(example_get) {
    if (!check_method(req, bulgogi::http::verb::get, res)) return;

    // Parse URL query: /example_get?name=xyz
    std::string target = std::string(req.target());
    auto pos = target.find('?');
    std::string name = "anonymous";
    if (pos != std::string::npos) {
        std::string query = target.substr(pos + 1);
        auto equal_pos = query.find('=');
        if (equal_pos != std::string::npos && query.substr(0, equal_pos) == "name") {
            name = query.substr(equal_pos + 1);
        }
    }

    bulgogi::set_json(res, {
            {"message", "Hello " + name}
    });
}

// === Example POST ===
REGISTER_VIEW(example_post) {
    if (!check_method(req, bulgogi::http::verb::post, res)) return;

    try {
        auto obj = bulgogi::get_json_obj(req);
        std::string name = obj.contains("name") ? obj["name"].as_string().c_str() : "anonymous";
        bulgogi::set_json(res, {
                {"message", "Received POST from " + name}
        });
    } catch (...) {
        bulgogi::set_json(res, {{"error", "Invalid JSON"}}, 400);
    }
}

// === GET with query param: /example_with_param?param=abc ===
REGISTER_VIEW(example_with_param) {
    if (!bulgogi::check_method(req, bulgogi::http::verb::get, res)) return;

    auto param = bulgogi::get_query_param(req, "param").value_or("none");

    bulgogi::set_json(res, {
        {"route", "example_with_param"},
        {"param", param}
    });
}


// === HEAD request example ===
REGISTER_VIEW(example_head) {
    if (!check_method(req, bulgogi::http::verb::head, res)) return;

    res.result(bulgogi::http::status::ok);
    res.set(bulgogi::http::field::content_type, "text/plain");
    res.content_length(0); // bodiless for HEAD
}

// === Multiple components in URL ===
REGISTER_VIEW(api, user, info) {
    if (!check_method(req, bulgogi::http::verb::get, res)) return;

    set_json(res, {
            {"user",   "bulgogi-eater"},
            {"status", "ok"},
            {"route",  "api/user/info"}
    });
}
*/
