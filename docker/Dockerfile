# === Build-time arguments ===
ARG COMPILER=clang
ARG APP=BulgogiAPP
ARG PORT=8080
ARG TIMEOUT=10
ARG CORS_MAX_AGE=86400
ARG NO_CORS=OFF


# === Base image ===
FROM ubuntu:22.04
LABEL authors="jeonghan-bae[bulgogi-framework organization]"

# Re-declare args in this stage
ARG COMPILER
ARG APP
ARG PORT
ARG TIMEOUT
ARG CORS_MAX_AGE
ARG NO_CORS

ENV DEBIAN_FRONTEND=noninteractive

# === Common system dependencies ===
RUN apt-get update && apt-get install -y \
    wget git curl pkg-config ca-certificates \
    build-essential cmake ninja-build \
    && rm -rf /var/lib/apt/lists/*

# === Conditionally install compiler toolchain ===
RUN if [ "$COMPILER" = "clang" ]; then \
        apt-get update && apt-get install -y clang lld llvm libc++-dev libc++abi-dev && \
        export CC=clang && export CXX=clang++; \
    elif [ "$COMPILER" = "gcc" ]; then \
        apt-get update && apt-get install -y gcc g++ && \
        export CC=gcc && export CXX=g++; \
    else \
        echo "Unsupported compiler: $COMPILER" && exit 1; \
    fi && \
    rm -rf /var/lib/apt/lists/*


# === Install CMake from source ===
ARG CMAKE_VER=3.27.9
WORKDIR /tmp/cmake
RUN wget https://github.com/Kitware/CMake/releases/download/v${CMAKE_VER}/cmake-${CMAKE_VER}.tar.gz \
    && tar -xzf cmake-${CMAKE_VER}.tar.gz \
    && cd cmake-${CMAKE_VER} \
    && ./bootstrap --parallel=$(nproc) \
    && make -j$(nproc) \
    && make install \
    && cd / && rm -rf /tmp/cmake

# === Build Boost 1.88.0 ===
ARG BOOST_VER=1.88.0
WORKDIR /tmp/boost
RUN wget https://github.com/boostorg/boost/releases/download/boost-${BOOST_VER}/boost-${BOOST_VER}-b2-nodocs.tar.gz \
    && tar xf boost-${BOOST_VER}-b2-nodocs.tar.gz \
    && cd boost-${BOOST_VER} \
    && ./bootstrap.sh \
    && m=$(nproc) && m=$(( m > 4 ? m - 2 : m )) && m=$(( m < 2 ? 2 : m )) \
    && ./b2 -j${m} install toolset=${COMPILER} \
    && cd / && rm -rf /tmp/boost

# === Build jh-toolkit POD ===
ARG JH_BRANCH=1.3.x-LTS
WORKDIR /tmp/build
RUN git clone --branch ${JH_BRANCH} --depth=1 https://github.com/JeongHan-Bae/jh-toolkit.git

WORKDIR /tmp/build/jh-toolkit
RUN cmake -G Ninja -B build-pod -DCMAKE_BUILD_TYPE=Release -DTAR=POD \
    && cmake --build build-pod \
    && cmake --install build-pod

# === Copy entire source from Dockerfile's parent directory ===
WORKDIR /tmp/${APP}
COPY ./ ./

# === Build your project ===
RUN cmake -G Ninja -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DAPP=${APP} \
    -DPORT=${PORT} \
    -DTIMEOUT=${TIMEOUT} \
    -DCORS_MAX_AGE=${CORS_MAX_AGE} \
    -DNO_CORS=${NO_CORS} \
    && cmake --build build

# === Package and cleanup ===
RUN mkdir -p /usr/app \
    && mv build/${APP} /usr/app/${APP}

RUN rm -rf /tmp/build

# === Final image ===
WORKDIR /usr/app
EXPOSE ${PORT}
ENV APP=${APP}
ENV LD_LIBRARY_PATH=/usr/local/lib
CMD ["sh", "-c", "/usr/app/${APP}"]
