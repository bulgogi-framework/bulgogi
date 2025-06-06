/// Copyright (c) 2025 bulgogi-framework
/// SPDX-License-Identifier: MIT
// main.cpp

#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/json.hpp>
#include <iostream>
#include <unordered_map>
#include <sstream>
#include <thread>
#include <csignal>
#include <atomic>
#include <vector>
#include <mutex>
#include "Web/views.hpp"


namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace json = boost::json;

using tcp = boost::asio::ip::tcp;

std::atomic g_should_exit = false;
std::unique_ptr<tcp::acceptor> global_acceptor;

std::vector<std::thread> session_threads;
std::mutex session_mutex;

void handle_signal(const int signal) {
    if (signal == SIGTERM || signal == SIGINT) {
        if (global_acceptor) {
            boost::system::error_code ec;
            auto err = global_acceptor->close(ec);
            (void) err;
        }
    }
}

std::unordered_map<std::string, views::HandlerFunc> build_route_map() {
    std::unordered_map<std::string, views::HandlerFunc> map;
    for (const auto &[name, func]: views::function_map) {
        map["/" + name] = func;
    }
    return map;
}

void handle_request(
        const std::unordered_map<std::string, views::HandlerFunc>& route_map,
        const http::request<http::string_body>& req,
        http::response<http::string_body>& res) {

    res.version(req.version());
    res.keep_alive(req.keep_alive());

    const std::string target = std::string(req.target());
    const auto query_pos = target.find('?');
    const std::string route = query_pos == std::string::npos ? target : target.substr(0, query_pos);

    // === Special handling for OPTIONS preflight ===
    if (req.method() == http::verb::options) {
        if (views::has_route(route)) {
            try {
                views::check_head(req);  // allow filtering on Origin / Headers
                res.result(http::status::no_content);
            } catch (const std::exception& e) {
                bulgogi::set_json(res, {
                        {"error", std::string("CORS preflight rejected: ") + e.what()}
                }, 403);
            }
            bulgogi::apply_cors(res);  // always include CORS headers
        } else {
            bulgogi::set_text(res, "404 Not Found (CORS preflight): " + route, 404);
            bulgogi::apply_cors(res);  // optional for visibility
        }
        return;
    }

    // === Regular request handling ===
    auto it = route_map.find(route);
    if (it != route_map.end()) {
        bulgogi::Response hres;

        try {
            it->second(req, hres);
        } catch (const std::exception& e) {
#ifndef NDEBUG
            bulgogi::set_json(hres, {{"error", e.what()}}, 400);
#else
            bulgogi::set_json(hres, {{"error", "Internal Server Error"}}, 500);
#endif
        }

        res = std::move(hres);
    } else {
        bulgogi::set_text(res, "404 Not Found: " + route, 404);
    }
}

void do_session(tcp::socket socket,
                const std::shared_ptr<const std::unordered_map<std::string, views::HandlerFunc>> &route_map) {
    try {
        beast::flat_buffer buffer;
        http::request<http::string_body> req;

        beast::tcp_stream stream(std::move(socket));
        stream.expires_after(std::chrono::seconds(TIMEOUT));

        http::read(stream, buffer, req);

        if (g_should_exit) return;

        http::response<http::string_body> res;
        handle_request(*route_map, req, res);

        http::write(stream, res);

        boost::system::error_code ec;
        auto& sock = stream.socket();

        const auto& result = sock.shutdown(tcp::socket::shutdown_send, ec);
        // Reference of ec, nodiscard
        if (result && result != boost::asio::error::not_connected) {
            std::cerr << "Shutdown failed: " << ec.message() << std::endl;
        }

    } catch (const beast::system_error &e) {
        if (!g_should_exit && e.code() != http::error::partial_message) {
            if (e.code() == http::error::end_of_stream) {
                std::clog << "[debug] Client disconnected\n";
            } else {
                std::cerr << "Session error: " << e.what() << std::endl;
            }
        }
        // Force shutdown silently
    } catch (const std::exception &e) {
        if (!g_should_exit) {
            std::cerr << "Session exception: " << e.what() << std::endl;
        }
    }
}


int main() {
    std::signal(SIGINT, handle_signal);
    std::signal(SIGTERM, handle_signal);
    views::init();

    auto route_map = std::make_shared<std::unordered_map<std::string, views::HandlerFunc>>(build_route_map());
    std::cout << "Registered routes:" << std::endl;
    for (const auto &[name, _]: *route_map) {
        std::cout << name << std::endl;
    }

    try {
        net::io_context ioc{1};
        global_acceptor = std::make_unique<tcp::acceptor>(ioc, tcp::endpoint{tcp::v4(), PORT});

        auto io_runner = [&ioc]() {
            ioc.run();
        };
        std::thread(std::move(io_runner)).detach();

        std::cout << "HTTP server running on port " STR(PORT) "..." << std::endl;

        while (!g_should_exit) {
            tcp::socket socket{ioc};
            boost::system::error_code ec;
            auto err = global_acceptor->accept(socket, ec);
            (void) err;

            if (ec == boost::asio::error::operation_aborted) break;

            if (ec) {
                std::cerr << "Accept error: " << ec.message() << std::endl;
                continue;
            }

            auto session = [sock = std::move(socket), route_map]() mutable {
                do_session(std::move(sock), route_map);
            };
            std::lock_guard<std::mutex> lock(session_mutex);
            session_threads.emplace_back(std::move(session));
        }

        ioc.stop();
        global_acceptor.reset();

        {
            std::lock_guard lock(session_mutex);
            for (size_t i = 0; i < session_threads.size(); ++i) { // NOLINT ranged-for illegal
                if (session_threads.at(i).joinable()) {
                    session_threads.at(i).join();
                }
            }
        }

        std::cout << "\U0001F44B Server exiting, cleaning up...\n";
        views::atexit();

    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        views::atexit();
        // Fallback to clean-up if exception occurs
        return 1;
    }

    return 0;
}
