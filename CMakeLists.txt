cmake_minimum_required(VERSION 3.20)
project(kard LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
set(CPR_USE_SYSTEM_CURL ON)
FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 1.10.5
)
FetchContent_MakeAvailable(cpr)

find_package(nlohmann_json REQUIRED)
find_package(Threads REQUIRED)

add_executable(kard brain/OllamaBrain.cpp core/kardcore.cpp brain/IInput.cpp brain/IOutput.cpp body/systemMonitor.cpp body/scheduler.cpp body/snapshot_log.cpp body/paths.cpp selfupdate/selfUpdate.cpp)
target_include_directories(kard PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(kard PRIVATE SNAPSHOT_BIN="$<TARGET_FILE:kard-snapshot>" CONFIG_PATH="${CMAKE_CURRENT_SOURCE_DIR}/brain/config.json")

target_link_libraries(kard PRIVATE cpr::cpr nlohmann_json::nlohmann_json)

add_executable(kard-snapshot body/snapshot_main.cpp body/snapshot_log.cpp body/systemMonitor.cpp body/paths.cpp)
target_include_directories(kard-snapshot PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(kard-snapshot PRIVATE nlohmann_json::nlohmann_json Threads::Threads)
