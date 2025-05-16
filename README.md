<div align="center" style="margin-top: 20px; margin-bottom: -30px;">
  <img src="https://raw.githubusercontent.com/bulgogi-framework/.github/main/res/img/Bulgogi.svg" alt="bulgogi logo" style="max-width: 100%; max-height: 1024px;">
</div>

<h1 align="center">Bulgogi</h1>

<p align="center">
  <b>A lightweight, web-free C++ HTTP API framework built on Boost.Beast</b>
</p>

<p align="center">
  <a href="https://github.com/bulgogi-framework/bulgogi/blob/main/docs/quick_start.md">
    <img src="https://img.shields.io/badge/Quick%20Start-black?style=for-the-badge&logo=fastapi" alt="Quick Start"/>
  </a>
</p>

---
[![C++20](https://img.shields.io/badge/C%2B%2B-20-violet.svg)](https://en.cppreference.com/w/cpp/20)
[![Boost](https://img.shields.io/badge/Boost-1.88-blue.svg)](https://www.boost.org/)
[![jh-toolkit](https://img.shields.io/badge/jh--toolkit-1.3.x--LTS-brightgreen)](https://github.com/JeongHan-Bae/JH-Toolkit/tree/1.3.x-LTS)
[![CMake](https://img.shields.io/badge/CMake-3.25%2B-blue)](https://cmake.org/)
[![Clang](https://img.shields.io/badge/Clang-14.0%2B-yellow)](https://clang.llvm.org/)
[![Docker](https://img.shields.io/badge/Docker-Buildx-lightgrey)](https://docs.docker.com/buildx/working-with-buildx/)
[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Contributors](https://img.shields.io/github/contributors/bulgogi-framework/bulgogi.svg)](https://github.com/bulgogi-framework/bulgogi/graphs/contributors)

--- 

## 🔥 What is Bulgogi?

**Bulgogi** is a minimal C++ HTTP API framework designed for developers who want full control, fast iteration, and zero bloat.

It provides a macro-based routing system over Boost.Beast, focuses on HTTP-only APIs, and includes no frontend baggage.  
Ideal for:

- Backend service APIs
- Internal tools
- RPC-style servers
- Embedded or game server endpoints
- Interop with JS frontends or CLI clients

---

## ✨ Features

- ⚡ **One macro = one route**: add endpoints in seconds
- 🧩 **Web-free by default**: no templating or frontend assumptions
- 🐚 **Zero runtime complexity**: no coroutines, no frameworks, no abstractions
- 🐳 **Docker-ready**: development and runtime images supported
- 💡 **Debug-only root page**: safe for pure APIs
- 🛠️ Powered by **Boost.Beast** and **jh-toolkit**

---

## 🧩 Web-Free by Design

Bulgogi intentionally avoids the web layer:

- No HTML renderer, no templating DSL
- No static file system unless you implement one
- APIs are consumed via curl, Postman, JS clients, or internal systems
- Debug root page appears *only* when `NDEBUG` is not defined

---

## 📁 Project Structure

```text
.
├── CMakeLists.txt            # Root build file
├── main.cpp                  # Entry point (No need to modify)
├── Web/
│   ├── views.cpp             # Define your routes here
│   ├── views.hpp             # Route Macros (No need to modify)
│   ├── template.hpp          # Optional embedded HTML registry
│   └── ...                   # Other predefined functions
├── Application/              # Your service logic (custom)
├── Entities/                 # Optional data models (Strongly Recommend applying POD from jh-toolkit)
├── docker/
│   ├── Dockerfile
│   └── Dockerfile.runtime
├── docs/
│   ├── quick_start.md        # Installation and setup guide
│   └── api.md                # Route registration and HTTP usage
├── NOTICE.md                 # License info for dependencies
├── build.md                  # Detailed build/deployment options
└── LICENSE                   # MIT license
```

---

## 📚 Documentation

* 📦 [Quick Start](docs/quick_start.md)
* 🔧 [Build & Deployment](docs/build.md)
* 📖 [API Reference](docs/api.md)

---

## 🍖 Why "Bulgogi"?

The name **"Bulgogi"** is a tongue-in-cheek reference to its foundation:
the framework is built on top of **Boost.Beast**, a powerful but low-level C++ HTTP library.

> Instead of fighting raw Beast — Bulgogi serves you the grilled version.

Just like the Joseonjok dish **bulgogi** (불고기), which means "grilled meat",
this project offers a **lightly wrapped, ready-to-serve experience**:

* No manual Beast session/state handling
* No deep Asio socket dance
* No template gymnastics or build-time complexity

Bulgogi is what Beast might feel like **if someone cooked it for you** —
clean, ready-to-use, and enjoyable, even for quick tasks.

You can still go low-level if you want —
but for most use cases, this "pre-grilled" framework is all you need.

---

## 📄 License

MIT License © 2025 [bulgogi-framework](https://github.com/bulgogi-framework)  
Bulgogi is open-sourced under permissive terms and welcomes contribution.

Created and maintained by [JeongHan-Bae](https://github.com/JeongHan-Bae)

---

## 🤝 Acknowledgements

* [Boost.Beast](https://github.com/boostorg/beast) — core HTTP engine  
* [jh-toolkit](https://github.com/JeongHan-Bae/jh-toolkit) — type-safe and pod utilities

See [NOTICE.md](./NOTICE.md) for third-party license disclosures.
