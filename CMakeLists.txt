cmake_minimum_required(VERSION 3.20)
project(okx_api)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
    add_definitions(-D_WIN32_WINNT=0x0A00 /bigobj)
else ()
    add_definitions(-fPIC)
endif ()

if (POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif ()

find_package(Boost 1.83 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(magic_enum REQUIRED)

include_directories(include vk_cpp_common/include SYSTEM ${Boost_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR})

if (NOT TARGET vk_common)
    add_subdirectory(vk_cpp_common)
endif()

set(HEADERS
        include/vk/okx/okx.h
        include/vk/okx/okx_models.h
        include/vk/okx/okx_futures_rest_client.h
        include/vk/okx/okx_http_session.h
        include/vk/okx/okx_event_models.h
        include/vk/okx/okx_futures_ws_client.h
        include/vk/okx/okx_futures_ws_session.h
        include/vk/okx/okx_ws_stream_manager.h)

set(SOURCES
        src/okx.cpp
        src/okx_event_models.cpp
        src/okx_futures_ws_client.cpp
        src/okx_futures_ws_session.cpp
        src/okx_ws_stream_manager.cpp
        src/okx_models.cpp
        src/okx_futures_rest_client.cpp
        src/okx_http_session.cpp)

if (MODULE_MANAGER)
    add_library(okx_api SHARED ${SOURCES} ${HEADERS})
else ()
    add_library(okx_api STATIC ${SOURCES} ${HEADERS})

    add_executable(okx_test test/main.cpp)
    target_link_libraries(okx_test PRIVATE spdlog::spdlog_header_only okx_api)
endif ()

target_link_libraries(okx_api PRIVATE OpenSSL::Crypto OpenSSL::SSL vk_common nlohmann_json::nlohmann_json)