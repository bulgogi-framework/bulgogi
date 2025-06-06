/// Copyright (c) 2025 bulgogi-framework
/// SPDX-License-Identifier: MIT


#pragma once

#include <string>
#include <unordered_map>
#include "bulgogi.hpp"
#include "marcos.hpp"


namespace views {

    using HandlerFunc = void (*)(const bulgogi::Request &req, bulgogi::Response &res);

    // Declare global function map
    extern std::unordered_map<std::string, HandlerFunc> function_map;

    /**
     * @brief Register a view handler for a nested URL path.
     *
     * This macro supports 1 to 5 path segments.
     * - The URL path is registered as a string like "a/b/c".
     * - The corresponding function name is generated using double underscores: a__b__c.
     * - Double underscores ("__") avoid naming conflicts with valid URLs like "a_b_c".
     *
     * Limitations:
     * - Supports between 1 and 5 segments.
     * - An empty path ("") is reserved for the root route.
     *   Use REGISTER_ROOT_VIEW(...) to register the root handler.
     */
#define REGISTER_VIEW(...) \
        void ROUTE_NAME(__VA_ARGS__)(const bulgogi::Request& req, bulgogi::Response& res); \
        struct EXPAND(ROUTE_NAME(__VA_ARGS__), _registrar) { \
            EXPAND(ROUTE_NAME(__VA_ARGS__), _registrar)() { \
                views::function_map[ROUTE_STR(__VA_ARGS__)] = ROUTE_NAME(__VA_ARGS__); \
            } \
        } EXPAND(ROUTE_NAME(__VA_ARGS__), _registrar_instance); \
        void ROUTE_NAME(__VA_ARGS__)(const bulgogi::Request& req, bulgogi::Response& res)

    /**
     * @brief Register one or more URL paths for a single handler function.
     *
     * This macro allows you to explicitly map multiple URL strings to a single handler.
     * It is useful when:
     * - You want to support "alias" routes (e.g., "user-info" and "user_info").
     * - The route string does not directly match the function name. (contains "-")
     * - You prefer manual control over automatic path generation.
     *
     * Usage:
     * - The first argument is the handler function name (which will be defined below).
     * - The remaining arguments are C-style string literals (e.g., "api/v1/info").
     * - Paths should NOT start with a leading slash ("/") — they are stored as-is.
     *
     * Example:
     * @code
     * REGISTER_VIEW_URLS(handle_user_info,
     *     "user-info",
     *     "user_info",
     *     "user/info"
     * ) {
     *     // your handler implementation here
     * }
     * @endcode
     *
     * Notes:
     * - This macro ensures the handler function is defined only once.
     * - Route conflicts are resolved by last-in-wins in the global function_map.
     * - You may mix this with REGISTER_VIEW() for full flexibility.
     */
#define REGISTER_VIEW_URLS(func_name, ...) \
        void func_name(const bulgogi::Request& req, bulgogi::Response& res); \
        struct func_name##_alias_registrar { \
            func_name##_alias_registrar() { \
                const char* paths[] = { __VA_ARGS__ }; \
                for (const auto& p : paths) views::function_map[p] = func_name; \
            } \
        } func_name##_alias_registrar_instance; \
        void func_name(const bulgogi::Request& req, bulgogi::Response& res)

    /**
     * @brief Register the handler for the root URL path ("/").
     *
     * This macro maps the given function to the root route `""`.
     * Root handlers can overwrite each other — the last one registered wins.
     * As long as function names differ, multiple root registrations are allowed.
     *
     * Notes:
     * - In debug mode, a default handler named `default_root` is registered to root.
     *   If you want to use your own root handler, either:
     *     - Use a different function name (e.g., `my_root`), or
     *     - Remove or comment out `default_root` in `views.cpp`.
     *
     * Example:
     * @code
     * REGISTER_ROOT_VIEW(my_root) {
     *         if (!check_method(req, bulgogi::http::verb::get, res)) return;
     *         bulgogi::set_html(res, your_html_str, 200);
     * }
     * @endcode
     */
#define REGISTER_ROOT_VIEW(func_name) REGISTER_VIEW_URLS(func_name, "")
}

namespace views {

    /// @brief Initialize the server with developer-defined logic
    void init();

    /// @brief Call this function to clean up resources before exiting
    void atexit();

    // Returns true if a view exists for this path
    inline bool has_route(std::string_view path) {
        if (path.empty()) return function_map.contains("");
        if (path[0] == '/') path.remove_prefix(1);
        return function_map.contains(std::string(path));
    }
}
