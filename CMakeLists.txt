cmake_minimum_required (VERSION 3.14)

include(cmake/options.cmake)

project (
    "cleanMqtt" 
    VERSION 0.0.1 
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

file(GLOB_RECURSE source_files "src/*.cpp")
file(GLOB_RECURSE  "include/cleanMqtt/*.h")

add_library (${PROJECT_NAME} ${source_files})
target_include_directories( ${PROJECT_NAME} PUBLIC include)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PUBLIC_API_EXPORT)
endif()

if(FORCE_ADD_PROPERTIES)
    target_compile_definitions(${PROJECT_NAME} PRIVATE FORCE_ADD_PROPERTIES)
endif()

if(ENABLE_WARNINGS_AS_ERRORS)
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
else()
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /WX-)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(${PROJECT_NAME} PRIVATE -Wno-error)
    endif()
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(BUILD_TESTS)
    add_subdirectory(tests)

    if(ENABLE_TESTS)
        enable_testing()
    endif()
endif()
