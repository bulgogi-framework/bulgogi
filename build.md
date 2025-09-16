<p align="right" style="margin-bottom: -30px;">
  <a href="https://github.com/bulgogi-framework/bulgogi#readme">
    <img src="https://img.shields.io/badge/Back%20to%20README-black?style=for-the-badge&logo=github" alt="Back to README"/>
  </a>
</p>

# 📦 Build & Deployment Guide

> This document explains how to build and run the **`bulgogi` project** under the 
> [`bulgogi-framework`](https://github.com/bulgogi-framework) organization using Docker or native build tools.  
> It supports both **development containers** and **slim production images**, while remaining compatible with
> local-native builds.

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
  --load \
  .
```

> ✅ Use `--build-arg COMPILER=gcc` to switch toolchains (default is `clang`).  
> ✅ On Apple Silicon or ARM servers, specify `--platform linux/arm64` (host and target **must** match).
>
> ⚠️ **Important (Must-Read):**
>
> * You **must** use `--load` — without it, `buildx` will not produce a locally runnable image.
> * You **must build on a 64-bit host with the same architecture as the target**:
>
>   * Cross-architecture builds **are not supported** (Boost will fail to compile).
>   * 32-bit hosts **are not supported** (`jh-toolkit` refuses to compile).
### ▶️ Run

```bash
docker run --rm -p 8080:8080 bulgogi-dev:latest
```

---

## 🚀 2. Runtime Image (`docker/Dockerfile.runtime`)

A **multi-stage minimal runtime image**, ideal for containerized production.

### ✅ Features

* **Only the final binary is included**
* **Statically linked Boost**
* **Header-only `jh-toolkit::pod`**
* Image size: \~70MB

### 🔧 Build

```bash
docker buildx build \
  --platform linux/amd64 \
  -t bulgogi-app:slim \
  -f docker/Dockerfile.runtime \
  --build-arg APP=BulgogiAPP \
  --build-arg PORT=8080 \
  --load \
  .
```

> ⚠️ **Important (Must-Read):**
>
> * `--load` is mandatory for a usable local image.
> * Builds must run on a **64-bit host with the same architecture as the target** — no exceptions.

### ▶️ Run

```bash
docker run --rm -p 8080:8080 bulgogi-app:slim
```

---

## ⚙️ 🔧 Build-Time Arguments (for Both Dockerfiles)

The following `--build-arg` options can be passed to both Dockerfiles to control behavior at **build time**.

> ❗ You **must** specify either `-f docker/Dockerfile` or `-f docker/Dockerfile.runtime`.  
> Running `docker build .` without `-f` will not work correctly.

| Argument       | Default     | Example                         | Description                                                             |
|----------------|-------------|---------------------------------|-------------------------------------------------------------------------|
| `COMPILER`     | `clang`     | `--build-arg COMPILER=gcc`      | Compiler toolchain (`clang` or `gcc`). `clang` is officially supported. |
| `APP`          | `APP`       | `--build-arg APP=MyServer`      | CMake project name and output binary name                               |
| `PORT`         | `8080`      | `--build-arg PORT=9000`         | Server listening port (compile-time constant)                           |
| `TIMEOUT`      | `10`        | `--build-arg TIMEOUT=15`        | Request timeout, in seconds                                             |
| `CORS_MAX_AGE` | `86400`     | `--build-arg CORS_MAX_AGE=3600` | Cache duration for CORS preflight results                               |
| `NO_CORS`      | `OFF`       | `--build-arg NO_CORS=ON`        | Disables CORS handling (`add_compile_definitions(NO_CORS=1)`)           |
| `CMAKE_VER`    | `3.27.9`    | `--build-arg CMAKE_VER=3.29.0`  | Override CMake version (⚠ not recommended — compatibility may break)    |
| `BOOST_VER`    | `1.88.0`    | `--build-arg BOOST_VER=1.85.0`  | Override Boost version (⚠ must include `boost::json`)                   |
| `JH_BRANCH`    | `1.3.x-LTS` | `--build-arg JH_BRANCH=main`    | Use a different branch of `jh-toolkit`                                  |

---

### 🧠 Recommendation

> For most users, the defaults are tested and stable.  
> **Do not override `BOOST_VER`, `CMAKE_VER`, or `JH_BRANCH`** unless you know what you're doing — these are pinned for
> compatibility.

---

### 🧪 Example: Dev Image with CORS Disabled

```bash
docker build \
  -f docker/Dockerfile \
  --build-arg APP=TestServer \
  --build-arg PORT=9001 \
  --build-arg NO_CORS=ON \
  -t bulgogi-dev:custom .
```

### 🚀 Example: Runtime Image with GCC and Custom App Name

```bash
docker build \
  -f docker/Dockerfile.runtime \
  --build-arg APP=TestServer \
  --build-arg PORT=9001 \
  --build-arg COMPILER=gcc \
  -t bulgogi-app:slim .
```

---

## 📁 Build Context & Path Requirements

All Docker builds must be run from the **project root**:

* ✅ Correct: `docker build -f docker/Dockerfile .`
* ❌ Incorrect: `cd docker && docker build .`

Relative paths like `COPY ./ ./` rely on the entire root context.

---

## 📚 About Boost & jh-toolkit

### Boost

* Requires **Boost ≥ 1.80** (due to `boost::json`)
* Docker builds use Boost **1.88.0**
* Local builds must ensure `boost_json` and `boost_system` are discoverable

### jh-toolkit

* Uses [`1.3.x-LTS`](https://github.com/JeongHan-Bae/JH-Toolkit/tree/1.3.x-LTS), currently **1.3.2**
* **Two targets are built in Docker**:

    * **`POD`**

        * Header-only module
        * This is the *true dependency* of bulgogi
    * **`ALL`**

        * Includes both `jh-toolkit` headers and `jh-toolkit-impl`
        * Provides runtime implementation (e.g. `jh::immutable_str`)
        * Previously forced `-march=native`, now supports **same-arch cross-compilation**
* ✅ Both are prebuilt in the Docker environment for maximum compatibility
* ✅ Framework users can choose:

    * Link only against **`jh::jh-toolkit-pod`** (default, minimal)
    * Or explicitly depend on **`jh::jh-toolkit-impl`** if extra runtime features are needed

* ℹ️ For detailed CMake usage examples (e.g. choosing `jh-toolkit-pod` vs `jh-toolkit-impl`), please refer to the **[API Guide](docs/api.md)**.

---

## 🖥️ Native Build Option

Native builds are great for:

* Intranet deployments
* Minimal startup time
* Dynamic linking with system-installed Boost

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DAPP=MyServer -DPORT=9000
cmake --build .
```

---

## 🧼 Cleanup

Both Dockerfiles clean up intermediate files, build artifacts, and temp downloads after installation to reduce image
size.

---

## ✅ Summary (Updated)

| Method               | Build Tools | Boost             | Toolkit                  | Image Size | Usage                                |
|----------------------|-------------|-------------------|--------------------------|------------|--------------------------------------|
| `Dockerfile`         | Full        | Dynamic           | `POD,ALL`                | \~1.4 GB   | Development, Debug, Cross-compile OK |
| `Dockerfile.runtime` | Staged      | Static            | `POD,ALL`                | \~70 MB    | Production (Slim)                    |
| Native               | Manual      | Dynamic preferred | Optional (`POD` / `ALL`) | < app size | Intranet servers, embedded systems   |

