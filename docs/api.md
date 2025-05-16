<p align="right" style="margin-bottom: -30px;">
  <a href="https://github.com/bulgogi-framework/bulgogi#readme">
    <img src="https://img.shields.io/badge/Back%20to%20README-black?style=for-the-badge&logo=github" alt="Back to README"/>
  </a>
</p>

# 📖 API Documentation

## 📌 Table of Contents

- [🧭 Route Definition](#-route-definition)
- [🧰 `bulgogi` Utilities](#-bulgogi-utilities)
- [📦 `jh::pod` Introduction](#-jhpod-introduction)

---

## 🧭 Route Definition

### 🪄 Registering Views

Use macros to declare routes concisely:

```c++
REGISTER_VIEW(ping) {
    if (!bulgogi::check_method(req, bulgogi::http::verb::get, res)) return;
    bulgogi::set_json(res, {{"status", "alive"}});
}
```

This binds `/ping` to the handler above.

---

### 🔁 `REGISTER_VIEW_URLS` — Explicit Route Binding

```c++
REGISTER_VIEW_URLS(my_handler,
    "user-info",
    "user_info",
    "user/info"
) {
    // handler code
}
```

Use this macro when:

* ✅ Your URL path contains characters that can't appear in function names (e.g. `-`)
* ✅ You want **multiple equivalent routes** to point to the same handler
* ✅ You need a path with **more than 5 segments** (which `REGISTER_VIEW(...)` does not support)

#### Key Differences vs `REGISTER_VIEW(...)`:

| Feature                           | `REGISTER_VIEW(...)` | `REGISTER_VIEW_URLS(...)` |
|-----------------------------------|----------------------|---------------------------|
| Function name auto-mapped to path | ✅ Yes                | ❌ Manual only             |
| Supports `-` in URL path          | ❌ No                 | ✅ Yes                     |
| Supports more than 5 segments     | ❌ No (1–5 max)       | ✅ Unlimited               |
| Multiple equivalent paths         | ❌ One path only      | ✅ Multi-path registration |
| Easier to read for custom routes  | 🚫 Limited           | ✅ Recommended for aliases |

#### Example: Route with hyphen and slashes

```c++
REGISTER_VIEW_URLS(get_user_info,
    "user-info",     // kebab-case alias
    "user_info",     // snake_case
    "user/info"      // REST-style
) {
    if (!bulgogi::check_method(req, bulgogi::http::verb::get, res)) return;
    std::string id = bulgogi::get_query_param(req, "id").value_or("unknown");
    bulgogi::set_json(res, {{"user", id}});
}
```

---

💡 Tip: You can mix `REGISTER_VIEW(...)` for simple routes and `REGISTER_VIEW_URLS(...)` for special cases.


### Supported macros:

| Macro                             | URL Path       | Notes                                |
|-----------------------------------|----------------|--------------------------------------|
| `REGISTER_VIEW(ping)`             | `/ping`        | Normal route                         |
| `REGISTER_ROOT_VIEW(main)`        | `/`            | Root fallback (debug)                |
| `REGISTER_VIEW(api, user, id)`    | `/api/user/id` | Multi-part route                     |
| `REGISTER_VIEW_URLS(f, paths...)` | Custom         | Manual control, aliases, `-` support |

Multi-part routes are joined with `/`, and function name becomes `api__user__id`.

### 🧠 When to Use Which Macro?

Use this table to decide which macro to use:

| Use Case                                   | Recommended Macro         |
|--------------------------------------------|---------------------------|
| Simple routes, function name matches       | `REGISTER_VIEW(...)`      |
| Root path `/`                              | `REGISTER_ROOT_VIEW(...)` |
| URL has `-` or doesn't match function name | `REGISTER_VIEW_URLS(...)` |
| Multiple equivalent routes                 | `REGISTER_VIEW_URLS(...)` |
| Deep routes (> 5 segments)                 | `REGISTER_VIEW_URLS(...)` |

---

### 🧪 Handler Basics

Each route receives:

```c++
void my_handler(const bulgogi::Request& req, bulgogi::Response& res);
```

Use utility functions to simplify logic:

```c++
if (!bulgogi::check_method(req, http::verb::post, res)) return;
auto obj = bulgogi::get_json_obj(req);
```

---

### 🔍 Examples

#### GET with query parameter

```c++
// GET /greet?name=John
REGISTER_VIEW(greet) {
if (!bulgogi::check_method(req, bulgogi::http::verb::get, res)) return;
std::string name = bulgogi::get_query_param(req, "name").value_or("anonymous");
bulgogi::set_json(res, {{ "hello", name }});
}
```

#### POST with JSON body

```c++
REGISTER_VIEW(echo) {
    if (!bulgogi::check_method(req, bulgogi::http::verb::post, res)) return;
    auto obj = bulgogi::get_json_obj(req);
    bulgogi::set_json(res, {{"you_said", obj["msg"]}});
}
```

> ⚠️ Will throw on invalid JSON — suggest wrap in try-catch or validate `req.body()` first.

#### HEAD route

```c++
REGISTER_VIEW(status) {
    if (!bulgogi::check_method(req, bulgogi::http::verb::head, res)) return;
    res.result(bulgogi::http::status::ok);
    res.set(bulgogi::http::field::content_type, "text/plain");
    res.content_length(0);
}
```

---

## 🧰 `bulgogi` Utilities

The `bulgogi` namespace wraps essential functionality from Boost.Beast.

### 🔎 Types

```c++
using bulgogi::Request;   // HTTP request (string body)
using bulgogi::Response;  // HTTP response
```

### ✔️ Method Check

```c++
bool bulgogi::check_method(const Request& req, http::verb expected, Response& res);
```

Returns `false` and responds with `405` JSON if mismatched.

### 📤 Response Utilities

```c++
bulgogi::set_json(res, {{"ok", true}});
bulgogi::set_text(res, "hello");
bulgogi::set_html(res, "<h1>Hi</h1>");
bulgogi::set_binary(res, raw_data, "dump.bin");
```

### 🌐 CORS & Redirect

```c++
bulgogi::apply_cors(res);                          // Adds Access-Control-* headers
bulgogi::set_redirect(res, "/home", 302);          // Redirects with Location
```

### 🔍 Request Helpers

```c++
auto json = bulgogi::get_json_obj(req);            // Parses body to boost::json::object
auto name = bulgogi::get_query_param(req, "q");    // Extracts ?q= from URL
```

---

### ⚙️ `views.cpp` Custom Hooks

You may optionally implement the following functions in `views.cpp` to add global logic:

```c++
namespace views {
    void init();   // Called once during startup
    void atexit(); // Called once on shutdown
    void check_head(const bulgogi::Request& req); // Called before CORS headers are applied
}
```

#### 🔸 `void views::init()`

```c++
void views::init() {
    /// Todo: Add initialization code if needed (e.g. DB connections, logging setup)
}
```

Called **once during application startup**, before any routes are registered.

---

#### 🔸 `void views::atexit()`

```c++
void views::atexit() {
    /// Todo: Add cleanup code if needed (e.g. closing resources)
}
```

Called **once at shutdown**, before exiting the application.

---

#### 🔸 `void views::check_head(const bulgogi::Request& req)`

```c++
void views::check_head([[maybe_unused]] const bulgogi::Request& req) {
    /// Todo: Implement global CORS/token validation if needed
    // Example: throw std::runtime_error("Unauthorized") if missing Authorization
}
```

Called automatically in `bulgogi::apply_cors()`.

* Throw an exception here to **block unauthorized requests**.
* The framework will catch it and return a `401 Unauthorized` response automatically.

---

> ✅ These are **optional**. Empty implementations are valid.

---

### 📥 Download Support

Download text as a file with content-type:

```c++
namespace download_types {
    constexpr jh::pod::array<char, 32> CSV = {"csv"};
}
using download_csv = bulgogi::set_download<download_types::CSV>;

REGISTER_VIEW(download_data) {
    std::string csv = "name,score\nAlice,90\nBob,88\n";
    download_csv::apply(res, csv, "result.csv");
}
```

---

### ⚠️ Runtime Error Handling

By default, route execution in `main` is wrapped with an exception catcher:

```c++
try {
    it->second(req, hres);
} catch (const std::exception &e) {
#ifndef NDEBUG
    bulgogi::set_json(hres, {{"error", e.what()}}, 400); // Debug: treat as client-side issue
#else
    bulgogi::set_json(hres, {{"error", "Internal Server Error"}}, 500); // Release: production-safe
#endif
}
```

#### Implications:

* In **Debug mode**, unhandled exceptions return `400` with the error message (helps during dev).
* In **Release mode**, the same code returns `500` with a generic message (avoids leaking internals).

---

### ✅ Best Practices

* **Always catch expected exceptions inside your handler** to generate controlled responses.

  ```c++
  REGISTER_VIEW(do_something) {
      try {
        // risky operation
      } catch (const std::runtime_error& err) {
          // You can build a custom message instead of exposing err.what()
          std::string err_str = std::string("Failed to process item: ") + std::to_string(item_id);
          bulgogi::set_json(res, {{"error", err_str}}, 400);
          return;
      }
  ```

* **Never deploy with `NDEBUG` undefined** — Debug mode reveals internal logic and stack traces.

* Treat `500` as a **last-resort, severe server fault**. Don't rely on global `catch` to handle logic errors.

---

📌 You are responsible for graceful handling in production.
This framework is intentionally minimal and avoids runtime safety layers.

---

## 📦 `jh::pod` Introduction

`jh::pod` is a modern C++20 system for defining and working with **plain old data** (POD) safely and portably.

### 💡 Why It Matters

| Advantage                   | Benefit                       |
|-----------------------------|-------------------------------|
| Trivial layout              | Zero-cost copying             |
| No constructors/destructors | Works with `memcpy`, `mmap`   |
| Fixed-size layout           | Cross-platform deterministic  |
| Type-safe utilities         | Structs, pairs, arrays, flags |

---

### 🧱 Defining POD

```c++
// Only pod-like attributes are allowed, otherwise compile error
JH_POD_STRUCT(User,
    uint32_t id;
    jh::pod::array<char, 16> name; // wrapper for char[16], better locality than std::string
);
// Only attributes and a default operator==() are generated
```

Use `JH_ASSERT_POD_LIKE(Type)` to validate third-party types.

---

### 🔧 Included Types

* `pod::pair<T1, T2>` a pod version of `std::pair`
* `pod::optional<T>`  a pod version of `std::optional`
* `pod::array<T, N>`  a wrapper for `T[N]` with STL-like interface
* `pod::bitflags<N>`  a container for bitwise flags
* `pod::bytes_view`   a view of raw bytes, offers `clone<T>()` for copying and `hash()` for hashing
* `pod::string_view`  a view of string-like data, offers basic string operations
* `pod::span<T>`      a view of contiguous data, similar to `std::span`

---

### 🧪 Example: `bitflags`

```c++
jh::pod::bitflags<32> flags{};
flags.set(4);
if (flags.has(4)) { /* logic */ }
```

---

### 📤 Memory View & Cloning

```c++
struct Packet { uint16_t id; uint8_t code; };
JH_ASSERT_POD_LIKE(Packet);

Packet p{10, 2};
auto view = jh::pod::bytes_view::from(p);
Packet copy = view.clone<Packet>(); // clone can only be applied to strict POD types
```

Also supports hashing, range access, and structured equality.

---

### ✅ STL Compatibility

All `jh::pod` types are compatible with `std::vector`, `std::unordered_map`, and Beast buffers.
You get `memcpy` speed + full type safety.

---

> 🔗 Learn more at [JH Toolkit repo](https://github.com/JeongHan-Bae/jh-toolkit)  
> 🔗 See [quick_start.md](quick_start.md) to get started  
> 🔗 See [build.md](../build.md) for CMake and Docker instructions
