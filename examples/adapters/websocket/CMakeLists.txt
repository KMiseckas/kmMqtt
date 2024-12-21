cmake_minimum_required (VERSION 3.14)
include(FetchContent)

project ("websocket" LANGUAGES CXX)

file(GLOB_RECURSE src_files "src/*.cpp")
add_library(${PROJECT_NAME} ${src_files})

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC cleanMqtt)