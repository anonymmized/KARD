cmake_minimum_required(VERSION 3.20)
project(kard LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(cpr REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Threads REQUIRED)

add_executable(kard brain/OllamaBrain.cpp core/kardcore.cpp brain/IInput.cpp brain/IOutput.cpp body/systemMonitor.cpp body/scheduler.cpp)
target_include_directories(kard PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(kard PRIVATE SNAPSHOT_BIN="$<TARGET_FILE:kard-snapshot>" CONFIG_PATH="${CMAKE_CURRENT_SOURCE_DIR}/brain/config.json" LOG_PATH="${CMAKE_CURRENT_SOURCE_DIR}/brain/model_sys_log.jsonl")

target_link_libraries(kard PRIVATE cpr::cpr nlohmann_json::nlohmann_json)

add_executable(kard-snapshot body/snapshot_main.cpp body/snapshot_log.cpp body/systemMonitor.cpp)
target_include_directories(kard-snapshot PRIVATE ${CMAKE_SOURCE_DIR})
target_compile_definitions(kard-snapshot PRIVATE LOG_PATH="${CMAKE_CURRENT_SOURCE_DIR}/brain/model_sys_log.jsonl")
target_link_libraries(kard-snapshot PRIVATE nlohmann_json::nlohmann_json Threads::Threads)
