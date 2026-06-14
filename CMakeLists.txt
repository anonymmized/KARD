cmake_minimum_required(VERSION 3.20)
project(kard LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(cpr REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Threads REQUIRED)

add_executable(kard brain/OllamaBrain.cpp core/kardcore.cpp brain/IInput.cpp brain/IOutput.cpp body/systemMonitor.cpp)
target_include_directories(kard PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(kard PRIVATE CONFIG_PATH="${CMAKE_CURRENT_SOURCE_DIR}/brain/config.json")

target_link_libraries(kard PRIVATE cpr::cpr nlohmann_json::nlohmann_json)
