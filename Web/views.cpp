/**
 * @file views.cpp
 * @brief Entry point for registering views and providing builtin server handlers.
 *
 * This file initializes and registers builtin HTTP handlers for the Bulgogi server,
 * such as `/ping` and `/shutdown_server`, as well as optional debug-time fallbacks like the root page.
 *
 * It also demonstrates how to register application-specific views using the `REGISTER_VIEW` macro,
 * and includes example handlers (commented out) for GET/POST/query parsing, etc.
 *
 * @section builtin_views Builtin Views
 * The following routes are provided by default:
 * - `/ping` — returns server health status (GET)
 * - `/shutdown_server` — gracefully shuts down the server (POST)
 *
 * @section example_views Example Views (commented out)
 * The file includes several example handlers such as:
 * - `/example_get`
 * - `/example_post`
 * - `/api/user/info`
 *
 * These are not compiled by default. You may uncomment them for reference or testing.
 *
 * @section customization Application Integration
 * You may define your own views using `REGISTER_VIEW(...)` or override the root handler
 * via `REGISTER_ROOT_VIEW(...)` in debug mode. See the examples or framework docs for more info.
 */

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

/**
 * @page
 * set_download
 * Example usage:
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

void views::check_head([[maybe_unused]] const bulgogi::Request &req) {
    /// Todo: Implement global check_head logic for cors if needed
    // Example: throw std::runtime_error("Unauthorized") if missing Authorization
    // dev: do nothing will authorize all preflight heads
}


/**
 * @section method_checking Method Enforcement and CORS
 *
 * The macro `check_method(...)` is the **recommended way** to enforce HTTP method validity
 * while ensuring correct CORS headers are automatically included in the response.
 *
 * Example usage:
 * @code
 * REGISTER_VIEW(update_user) {
 *     if (!check_method(req, bulgogi::http::verb::put, res)) return;
 *     // your logic here
 * }
 * @endcode
 *
 * @note
 * - **OPTIONS requests are automatically handled** by `check_method`. If the incoming request is
 *   a CORS preflight, it will respond with the correct headers and return `false` early, skipping
 *   your handler logic.
 * - **This check must be the first thing in your handler**, otherwise OPTIONS requests will continue
 *   into your logic unnecessarily.
 * - `apply_cors()` is internally called by `check_method`, so **you do not need to call it manually**.
 * - You can pass multiple allowed verbs (e.g., `check_method(req, {verb::get, verb::head}, res)`),
 *   but it is generally discouraged in RESTful design since each URL should ideally respond to a single verb.
 * - If the method is not allowed, a `405 Method Not Allowed` response will be returned automatically.
 *
 * Full signature:
 * @code
 * bool check_method(const Request& req,
 *                   std::initializer_list<http::verb> allowed_methods,
 *                   Response& res,
 *                   std::string_view allow_origin = "*");
 * @endcode
 */


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


/**
 * @page example_views HTTP Method Examples
 * @brief Demonstrates typical usage of GET, POST, PUT, and HEAD views in Bulgogi.
 *
 * This page documents several example `REGISTER_VIEW` handlers to demonstrate
 * how to parse query parameters, JSON request bodies, and return structured responses.
 *
 * These examples are **not compiled by default**. You may uncomment or copy them
 * into your application as a quick reference or test scaffold.
 *
 * @section example_get GET with query string
 * Route: `/example_get?name=xyz`
 *
 * @code{.cpp}
 * REGISTER_VIEW(example_get) {
 *     if (!check_method(req, bulgogi::http::verb::get, res)) return;
 *
 *     // Parse URL query: /example_get?name=xyz
 *     auto name = bulgogi::get_query_param(req, "name").value_or("anonymous");
 *
 *     bulgogi::set_json(res, {
 *         {"message", "Hello " + name}
 *     });
 * }
 * @endcode
 *
 * @section example_post POST with JSON body
 * Route: `/example_post`, body: `{ "name": "bob" }`
 *
 * @code{.cpp}
 * REGISTER_VIEW(example_post) {
 *     if (!check_method(req, bulgogi::http::verb::post, res)) return;
 *
 *     try {
 *         auto obj = bulgogi::get_json_obj(req);
 *         std::string name = obj.contains("name") ? obj["name"].as_string().c_str() : "anonymous";
 *
 *         bulgogi::set_json(res, {
 *             {"message", "Received POST from " + name}
 *         });
 *     } catch (...) {
 *         bulgogi::set_json(res, {{"error", "Invalid JSON"}}, 400);
 *     }
 * }
 * @endcode
 *
 * @section example_head HEAD request
 * Route: `/example_head`
 *
 * @code{.cpp}
 * REGISTER_VIEW(example_head) {
 *     if (!check_method(req, bulgogi::http::verb::head, res)) return;
 *
 *     res.result(bulgogi::http::status::ok);
 *     res.set(bulgogi::http::field::content_type, "text/plain");
 *     res.content_length(0); // bodiless for HEAD
 * }
 * @endcode
 *
 * @section example_nested Nested route segments
 * Route: `/api/user/info`
 *
 * @code{.cpp}
 * REGISTER_VIEW(api, user, info) {
 *     if (!check_method(req, bulgogi::http::verb::get, res)) return;
 *
 *     set_json(res, {
 *         {"user",   "bulgogi-eater"},
 *         {"status", "ok"},
 *         {"route",  "api/user/info"}
 *     });
 * }
 * @endcode
 *
 * @section example_put PUT with query and JSON
 * Route: `/update_user?user=alice`, body: `{ "email": "...", "age": 30 }`
 *
 * @code{.cpp}
 * REGISTER_VIEW(update_user) {
 *     if (!check_method(req, bulgogi::http::verb::put, res)) return;
 *
 *     auto username = bulgogi::get_query_param(req, "user").value_or("");
 *
 *     if (username.empty()) {
 *         bulgogi::set_json(res, {{"error", "Missing user parameter"}}, 400);
 *         return;
 *     }
 *
 *     try {
 *         auto obj = bulgogi::get_json_obj(req);
 *         std::string email = obj.contains("email") ? obj["email"].as_string().c_str() : "";
 *         auto age = obj.contains("age") ? obj["age"].as_int64() : static_cast<int64_t>(-1);
 *
 *         bulgogi::set_json(res, {
 *             {"status", "updated"},
 *             {"user", username},
 *             {"email", email},
 *             {"age", age}
 *         });
 *
 *     } catch (...) {
 *         bulgogi::set_json(res, {{"error", "Invalid JSON in request body"}}, 400);
 *         return;
 *     }
 * }
 * @endcode
 *
 * @section register_view_urls Register multiple alias routes
 * Use `REGISTER_VIEW_URLS()` when you need to bind multiple route paths to the same handler function.
 *
 * @code{.cpp}
 * REGISTER_VIEW_URLS(handle_user_info,
 *     "user-info",
 *     "user_info",
 *     "user/info"
 * ) {
 *     if (!check_method(req, bulgogi::http::verb::get, res)) return;
 *     bulgogi::set_json(res, {
 *         {"alias", true},
 *         {"message", "Handled user info request"}
 *     });
 * }
 * @endcode
 *
 * This approach is useful when:
 * - You want to support both kebab-case and snake_case URLs.
 * - You prefer decoupling the route string from function naming.
 * - You need full control over what paths resolve to this handler.
 *
 * @note Do **not** prefix path strings with `/` — they're stored as-is and matched directly.
 *
 * @author
 * bulgogi-framework contributors
 */
