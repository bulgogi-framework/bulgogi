<p align="right" style="margin-bottom: -30px;">
  <a href="https://github.com/bulgogi-framework/bulgogi#readme">
    <img src="https://img.shields.io/badge/Back%20to%20README-black?style=for-the-badge&logo=github" alt="Back to README"/>
  </a>
</p>


# 📦 Build & Deployment Guide

> This document explains how to build and run the **`bulgogi` project** under the [`bulgogi-framework`](https://github.com/bulgogi-framework) organization using Docker or native build tools.
> It supports both **development containers** and **slim production images**, while remaining compatible with local-native builds.

---

## 🛠️ 1. Development Image (`docker/Dockerfile`)

A full Docker-based build environment for debugging, iteration, and testing.

### ✅ Features

* **Clang/LLVM Toolchain**
* **CMake 3.27.9** (built from source)
* **Boost 1.88.0** (with `json` support, built from source)
* **`jh-toolkit` POD module**, from [`1.3.x-LTS`](https://github.com/JeongHan-Bae/JH-Toolkit/tree/1.3.x-LTS)
* Full application source code

### 🔧 Build

```bash
docker buildx build \
  --platform linux/amd64 \
  -t bulgogi-dev:latest \
  -f docker/Dockerfile \
  .
```

You may use `--build-arg COMPILER=gcc` to switch to GCC.  
Use `--platform linux/arm64` if your native architecture is ARM64.  
You may also use `--no-cache` to force a clean build.  
You may use `docker build` instead of `docker buildx build` if you are not building on exactly the same architecture as the target.  

### ▶️ Run

```bash
docker run --rm -p 8080:8080 bulgogi-dev:latest
```

---

## 🚀 2. Runtime Image (`docker/Dockerfile.runtime`)

A **multi-stage minimal runtime image**, ideal for containerized production.

### ✅ Features

* **Only final executable included**
* **Statically linked Boost** (no `.so` required at runtime)
* **Header-only jh-toolkit::pod**, embedded at compile time
* Final image size: **\~70MB**

### 🔧 Build

```bash
docker buildx build \
  --platform linux/amd64 \
  -t bulgogi-app:slim \
  -f docker/Dockerfile.runtime \
  --build-arg APP=BulgogiAPP \
  --build-arg PORT=8080 \
  .
```

### ▶️ Run

```bash
docker run --rm -p 8080:8080 bulgogi-app:slim
```

---

## 📁 Build Context & Path Requirements

> 🧭 All `docker build` commands in this document **must be run from the project root directory**.

This is because:

* The `Dockerfile` is located in the `docker/` subdirectory
* The Docker build context expects the **entire source tree**
* Relative paths like `COPY ./ ./` and CMake references to rely on **root-relative layout**

✅ **Do not run builds from inside `docs/` or `docker/`** — the paths will break, and your application may not compile or run correctly.

---

## ⚙️ `docker build` vs `docker buildx`

When choosing between `docker build` and `docker buildx`, keep the following in mind:

| Feature           | `docker build`                       | `docker buildx`                                                               |
|-------------------|--------------------------------------|-------------------------------------------------------------------------------|
| Architecture      | Host-only                            | Same-architecture only (**do NOT cross-build**, since Boost must be compiled) |
| Optimization      | Full native (`-march=native`)        | Portable or emulated targets                                                  |
| Speed             | ⚡️ Fast (native toolchain)           | 🐢 Slightly slower (due to metadata + context handling)                       |
| Image Portability | ❌ Low (host-dependent instructions)  | ✅ High (portable across systems of the same architecture)                     |
| Use Case          | Local development, profiling, tuning | Reproducible builds, CI/CD pipelines, multi-platform releases                 |

> 🧠 **Tip:** If your machine and your deployment target are exactly the same architecture (e.g., `x86-64-v3`), `docker build` will produce **smaller and faster** images.
> Use `buildx` only when targeting multiple systems or publishing externally.

---

## 📚 About Boost & JSON

This project **requires Boost's `json` component**, which was introduced in **Boost 1.75+**, and considered stable from **1.80+**.

If building **outside Docker**, you must:

* Install **Boost ≥ 1.80** from [https://www.boost.org](https://www.boost.org)
* Ensure `boost_system` and `boost_json` are discoverable via CMake

You may use dynamic linking locally if you prefer smaller binaries.

---

## 🔧 About jh-toolkit

This project depends on [`jh-toolkit`](https://github.com/JeongHan-Bae/JH-Toolkit), licensed under Apache 2.0.
The image uses the `1.3.x-LTS` branch with the **POD (Plain Old Data)** module, which is:

* ✅ Fully template-based
* ✅ Header-only
* ✅ Compiled into your application

📄 Full documentation:
👉 [jh-toolkit POD Module](https://github.com/JeongHan-Bae/JH-Toolkit/blob/1.3.x-LTS/docs/pod.md)

> If you wish to use the **full version** of `jh-toolkit`, follow the instructions in its documentation, and update your `CMakeLists.txt` and Dockerfile accordingly (switch from `-DTAR=POD` to full build).

---

## 🖥️ Local Build (Native Compilation)

Docker is convenient for uniform environments, but **native builds offer**:

* Smaller local disk footprint
* Better startup performance (no container layer)
* Direct control over Boost dynamic/static linkage

For **intranet deployments with high request concurrency**, it is **strongly recommended** to:

* Build and deploy natively
* Use system-installed Boost with dynamic linking
* Run without Docker to reduce latency

---

## 📦 Summary

| Method               | Build Tools | Boost             | Toolkit                | Image Size | Usage                      |
|----------------------|-------------|-------------------|------------------------|------------|----------------------------|
| `Dockerfile`         | Full        | Dynamic           | POD                    | \~1.4 GB   | Development, Debug         |
| `Dockerfile.runtime` | Staged      | Static            | POD                    | \~70 MB    | Production (Slim)          |
| Native               | Manual      | Dynamic preferred | Optional (full or POD) | < app size | Intranet servers, embedded |

---

## 🧼 Cleanup

Both Dockerfiles clean up all sources after installation (CMake, Boost, jh-toolkit, build trees) to reduce final size.
