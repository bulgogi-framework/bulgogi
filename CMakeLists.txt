cmake_minimum_required(VERSION 3.25)

# Allow overriding executable name
if(NOT DEFINED APP OR APP STREQUAL "")
    set(APP "APP")
endif()


# Set project name based on APP if not already defined
project(${APP} LANGUAGES CXX)
cmake_policy(SET CMP0144 NEW)

# ==== Port (compile-time) ====
if(NOT DEFINED PORT)
    set(PORT 8080)
endif()

if(NOT PORT MATCHES "^[0-9]+$")
    message(FATAL_ERROR "PORT must be a number")
endif()

if(PORT LESS 1 OR PORT GREATER 65535)
    message(FATAL_ERROR "PORT must be between 1 and 65535")
endif()

# ==== TIMEOUT ====
if(NOT DEFINED TIMEOUT)
    set(TIMEOUT 10)
endif()

# ==== CORS_MAX_AGE ====
if(NOT DEFINED CORS_MAX_AGE)
    set(CORS_MAX_AGE 86400)
endif()

# ==== NO_CORS ====
option(NO_CORS "Disable CORS handling in server" OFF)

add_compile_definitions(PORT=${PORT})
add_compile_definitions(TIMEOUT=${TIMEOUT})
add_compile_definitions(CORS_MAX_AGE=${CORS_MAX_AGE})
if(NO_CORS)
    add_compile_definitions(NO_CORS=1)
endif()

# ==== Compiler flags ====
set(EXTRA_OPT_FLAGS "")
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    if(NOT CMAKE_CROSSCOMPILING)
        include(CheckCXXCompilerFlag)
        check_cxx_compiler_flag("-march=native" HAS_MARCH_NATIVE)
        if(HAS_MARCH_NATIVE)
            set(EXTRA_OPT_FLAGS "-march=native")
        endif()
    else()
        message(STATUS "Cross-compiling for processor: ${CMAKE_SYSTEM_PROCESSOR}")
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64)$")
            set(EXTRA_OPT_FLAGS "-march=x86-64-v3")
        elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64)$")
            set(EXTRA_OPT_FLAGS "-march=armv8-a")
        elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^armv7")
            set(EXTRA_OPT_FLAGS "-march=armv7-a")
        else()
            message(WARNING "Unknown target architecture: ${CMAKE_SYSTEM_PROCESSOR}, no -march set")
        endif()
    endif()

    message(STATUS "March OPT FLAG: ${EXTRA_OPT_FLAGS}")

    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        message(STATUS " Build type is Release")
        message(STATUS " Release optimization flags: ${CMAKE_CXX_FLAGS_RELEASE}")
    endif()

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-O2)
        message(STATUS " Build type is Debug")
        message(STATUS " Debug optimization flags: -O2")
    endif()

    add_compile_options(
            ${EXTRA_OPT_FLAGS}
            -ftree-vectorize
            -funroll-loops
            -fno-omit-frame-pointer
            -Wall -Wextra -Wpedantic
    )
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ==== Boost ====
if(UNIX)
    list(APPEND CMAKE_PREFIX_PATH /usr /usr/local)
    set(BOOST_ROOT "/usr")
    set(Boost_NO_SYSTEM_PATHS OFF)
    set(Boost_NO_BOOST_CMAKE ON)
endif()

find_package(Boost REQUIRED COMPONENTS system json)
find_package(jh-toolkit REQUIRED)

# ==== Sources ====
add_executable(${APP}
        main.cpp
        Web/views.cpp
)

# ==== Include & Link ====
include_directories(${Boost_INCLUDE_DIRS})

target_link_libraries(${APP}
        PRIVATE
        jh::jh-toolkit-pod
        ${Boost_LIBRARIES}
)
