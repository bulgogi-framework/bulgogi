<div align="center" style="margin-top: 20px; margin-bottom: -30px;">
  <img src="https://raw.githubusercontent.com/bulgogi-framework/.github/main/res/img/Bulgogi.svg" alt="bulgogi logo" style="max-width: 100%; max-height: 1024px;">
</div>

<h1 align="center">Bulgogi (朝鲜族烤肉)</h1>

<p align="center">
  <b>一个轻量级、无网页依赖的 C++ HTTP API 框架，基于 Boost.Beast</b>
</p>

<p align="center">
  <a href="https://github.com/bulgogi-framework/bulgogi/blob/main/docs/quick_start.md">
    <img src="https://img.shields.io/badge/Quick%20Start-black?style=for-the-badge&logo=fastapi" alt="快速上手"/>
  </a>
</p>

<p align="center">
  <a href="https://github.com/bulgogi-framework/bulgogi#readme">
    <img src="https://img.shields.io/badge/Back%20to%20English%20README-black?style=for-the-badge&logo=github" alt="Back to README"/>
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

## 🔥 什么是 Bulgogi？

**Bulgogi (朝鲜族烤肉)** 是一个极简的 C++ HTTP API 框架，专为那些追求完全控制、高速迭代、零冗余的开发者设计。

它基于 Boost.Beast 实现宏路由，专注于纯 HTTP 接口，不包含任何前端内容。
适用于：

* 后端服务 API
* 内部工具
* RPC 风格的服务端
* 嵌入式或游戏服务器接口
* 与 JS 前端或命令行客户端的交互

---

## ✨ 框架特色

* ⚡️ **一个宏 = 一个路由**：接口定义极简，开发高效
* 🧩 **默认无网页依赖**：无模板引擎，无前端假设
* 🐚 **零运行时复杂度**：无协程，无框架，无多层抽象
* 🐳 **Docker 友好**：支持开发镜像与运行时镜像
* 💡 **仅调试模式下启用根页面**：适合纯 API 项目
* 🛠️ 基于 **Boost.Beast** 和 **jh-toolkit**

---

## 🧩 原生无网页层

Bulgogi 有意避开传统 Web 层：

* 无 HTML 渲染器，无模板 DSL
* 无静态文件系统 (如需请自行实现)
* 接口使用 curl、Postman、JS 客户端或内部系统调用
* 根调试页面仅在未定义 `NDEBUG` 时显示

---

## 📁 项目结构

```text
.
├── CMakeLists.txt            # 顶层构建文件
├── main.cpp                  # 入口文件 (无需修改)
├── Web/
│   ├── views.cpp             # 路由定义入口
│   ├── views.hpp             # 路由宏 (无需修改)
│   ├── template.hpp          # 可选嵌入式 HTML 注册表
│   └── ...                   # 其他预定义方法
├── Application/              # 自定义业务逻辑
├── Entities/                 # 可选数据模型 (强烈建议使用 jh-toolkit 中的 POD)
├── docker/
│   ├── Dockerfile
│   └── Dockerfile.runtime
├── docs/
│   ├── quick_start.md        # 安装和上手指南
│   └── api.md                # 接口注册与 HTTP 用法
├── NOTICE.md                 # 第三方依赖许可信息
├── build.md                  # 构建/部署配置说明
└── LICENSE                   # MIT 许可协议
```

---

## 📚 文档索引

* 📦 [快速开始](docs/quick_start.md)
* 🔧 [构建与部署](build.md)
* 📖 [API 文档](docs/api.md)

---

## 🍖 为什么叫"Bulgogi"？

**"Bulgogi (불고기)"** 是对框架底层的一个轻松调侃：
该项目建立在 **Boost.Beast** 之上 —— 一个强大但底层的 C++ HTTP 库。

> 与其"生吃 Beast"，不如"烤熟再吃"。

就像朝鲜族料理中的 "bulgogi"("烤肉")，这个框架提供了一个 **轻封装、可即食** 的版本：

* 无需手动处理 Beast 的连接和状态
* 无需复杂 Asio 套接字操作
* 无模板魔法或构建时困境

Bulgogi 就像是 Beast **被人给你烤好了**：
干净、可用、轻松上手，适用于绝大多数需求。

当然你仍可以深入底层，
但对大多数用户来说，这个"预烤版本"已经足够了。

---

## 📄 许可协议

MIT 许可证 © 2025 [bulgogi-framework](https://github.com/bulgogi-framework)
本项目开源并欢迎贡献。

项目由 [JeongHan-Bae](https://github.com/JeongHan-Bae) 创建与维护。

---

## 🤝 鸣谢

* [Boost.Beast](https://github.com/boostorg/beast) — 核心 HTTP 引擎
* [jh-toolkit](https://github.com/JeongHan-Bae/jh-toolkit) — 类型安全和 POD 工具集

详细的第三方许可信息请见 [NOTICE.md](./NOTICE.md)
