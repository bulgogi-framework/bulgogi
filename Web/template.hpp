/// Copyright (c) 2025 bulgogi-framework
/// SPDX-License-Identifier: MIT

/**
 * @file template.hpp
 * @brief Contains the default fallback HTML page for Bulgogi server.
 *
 * This header defines a minimal default HTML page shown when no
 * `REGISTER_ROOT_VIEW` handler is registered. It is intended only for
 * debug or development environments to confirm that the server is running.
 *
 * @section customization Customization
 * Users may override this default page by assigning their own HTML content
 * to the `default_page::html` variable. This can be done directly using a
 * raw string literal (`R"__html__(...)__html__"`), without needing any template engine.
 *
 * @note Bulgogi does not provide any frontend rendering system or template
 *       engine. This fallback is deliberately minimal and intended only
 *       to assist in bootstrapping during development.
 *
 * @section rawstring Why Use `__html__`?
 * The raw string literal uses a custom delimiter `__html__` to prevent
 * accidental termination. For example, standard raw strings like `R"( ... )"`
 * may break if the HTML contains `)"`, which is common. Using a custom delimiter
 * such as `R"__html__( ... )__html__"` ensures correctness and clarity.
 *
 * @example
 * @code
 * if (!check_method(req, bulgogi::http::verb::get, res)) return;
 * bulgogi::set_html(res, default_page::html, 200);
 * @endcode
 *
 * @author
 * bulgogi-framework contributors
 */


#pragma once

#include <string>

namespace default_page {

    std::string html = R"__html__(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>Bulgogi Server</title>
  <style>
    body {
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
      text-align: center;
      padding: 3em;
      background-color: #fafafa;
      color: #333;
    }

    h1 {
      color: #e25822;
      font-size: 2.5em;
      margin-bottom: 0.2em;
    }

    p {
      font-size: 1.1em;
      margin: 0.8em 0;
    }

    code {
      background: #f4f4f4;
      padding: 0.3em 0.5em;
      border-radius: 5px;
      font-family: monospace;
    }

    small {
      display: block;
      margin-top: 2em;
      margin-left: 50%;
      text-align: left;
      font-size: 1em;
      color: #888;
    }

    .logo-wrapper {
      margin-top: 3em;
    }

    /* Dark mode support */
    @media (prefers-color-scheme: dark) {
      body {
        background-color: #1e1e1e;
        color: #ddd;
      }

      h1 {
        color: #ff7f50;
      }

      code {
        background: #2a2a2a;
        color: #ffcc99;
      }

      small {
        color: #aaa;
      }
    }
  </style>
</head>
<body>
  <h1>🍖 Bulgogi Web Server Installed</h1>
  <p>This is the default page because no <code>REGISTER_ROOT_VIEW</code> handler was defined.</p>
  <p>To override it, define a root view in your application code.</p>
  <p>This page is shown only in <strong>debug mode</strong>.</p>

  <small>&mdash; bulgogi-framework</small>

  <div class="logo-wrapper" align="center">
    <img src="https://raw.githubusercontent.com/bulgogi-framework/.github/main/res/img/Bulgogi.svg" style="max-width: 100%; max-height: 1024px;" />
  </div>
</body>
</html>
    )__html__";
}
