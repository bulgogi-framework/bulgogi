<p align="right" style="margin-bottom: -30px;">
  <a href="https://github.com/bulgogi-framework/bulgogi#readme">
    <img src="https://img.shields.io/badge/Back%20to%20README-black?style=for-the-badge&logo=github" alt="Back to README"/>
  </a>
</p>

## 🚀 Quick Start

### 1. Clone this repository

```sh
git clone https://github.com/bulgogi-framework/bulgogi.git
cd bulgogi
````

### 2. Install Dependencies

**Boost ≥ 1.80**

* On Ubuntu:

  ```sh
  sudo apt install libboost-all-dev
  ```

* On macOS (using Homebrew):

  ```sh
  brew install boost
  ```

* Or build manually:

  ```sh
  wget https://github.com/boostorg/boost/releases/download/boost-1.88.0/boost-1.88.0-b2-nodocs.tar.gz
  tar xf boost-1.88.0-b2-nodocs.tar.gz
  cd boost-1.88.0
  ./bootstrap.sh && ./b2 -j$(nproc) install
  ```

**CMake ≥ 3.25**

**[jh-toolkit](https://github.com/JeongHan-Bae/jh-toolkit)** — required dependency:
Choose one of the following installation methods:

* **Full source build:**

  ```sh
  git clone --branch 1.3.x-LTS --depth=1 https://github.com/JeongHan-Bae/jh-toolkit.git
  cd jh-toolkit
  cmake -B build-pod -DCMAKE_BUILD_TYPE=Release -DTAR=POD
  cmake --build build-pod
  sudo cmake --install build-pod
  ```

* **Header-only POD package:**

  ```sh
  wget https://github.com/JeongHan-Bae/JH-Toolkit/releases/download/JH-Toolkit-1.3.1/jh-toolkit-pod-1.3.1.tar.gz
  tar -xf jh-toolkit-pod-1.3.1.tar.gz
  # Move to a known include path or set CMAKE_PREFIX_PATH
  ```

* You may set `CMAKE_PREFIX_PATH` or use `-DCMAKE_TOOLCHAIN_FILE` if needed.

  > If installed to system default paths, CMake should find everything automatically.

### 3. Build and Run (Debug / Development)

**Terminal build:**

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
./build/APP
```

Server will start at:

```
http://localhost:8080
```

Alternatively, use **CLion** or **VSCode** for development.

### 4. Customize Your API

Edit `Web/views.cpp` to add or modify endpoints.
Place your business logic under directories like `Application/`, `Entities/`, etc.
To embed static HTML strings, register them in `Web/template.hpp`.

### 5. (Optional) Build & Run with Docker

**Development image:**

```sh
docker build -f docker/Dockerfile -t bulgogi-dev .
docker run --rm -p 8080:8080 bulgogi-dev
```

**Slim runtime image:**

```sh
docker build -f docker/Dockerfile.runtime -t bulgogi-runtime .
docker run --rm -p 8080:8080 bulgogi-runtime
```

---

## 📌 Notes

* This project is intended as a **starter template** — feel free to clone, rename, and customize.
* Primary development happens in `Web/views.cpp`; framework internals are rarely touched.
* Docker builds are optimized for release and strip debug symbols.
* Native builds (via Clang/GCC) offer faster iteration and more control.
* See [`build.md`](../build.md) for advanced build options and CI integration.

### 🧩 Web-Free by Default

Bulgogi is designed as a pure HTTP API backend framework — ideal for machine-to-machine communication, backend services, or RPC-style systems.

* No bundled HTML renderer or static file server
* No template engine, no client runtime
* **Only** in `Debug` mode will the root `/` route show a minimal HTML page for confirmation
* Suitable for integration with frontend apps (React/Vue), CLI tools, IoT, game servers, or embedded devices

If you want to serve HTML or static files, you're free to implement that in `views.cpp` or use your own logic.
