cmake_minimum_required (VERSION 3.14)

#Include initial CMake files
include(cmake/options.cmake)

project (
    "cleanMqtt" 
    VERSION 0.0.1 
    LANGUAGES CXX)

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:preprocessor")
endif()

file(GLOB_RECURSE source_files "src/*.cpp")

add_library (${PROJECT_NAME} ${source_files})
target_include_directories( ${PROJECT_NAME} PUBLIC include/public)
target_include_directories( ${PROJECT_NAME} PRIVATE include/private)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PUBLIC_API_EXPORT)
endif()

if(FORCE_ADD_PROPERTIES)
    target_compile_definitions(${PROJECT_NAME} PRIVATE FORCE_ADD_PROPERTIES)
endif()

if(ENABLE_LOGS)
    set(LOG_LEVEL_TARGET 0 STRING "Set the log level for which to print logs (TRACE, INFO, DEBUG, WARNING, ERROR).")
    target_compile_definitions(${PROJECT_NAME} PRIVATE 
    LOG_LEVEL=${LOG_LEVEL_TARGET}
    ENABLE_LOGS)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE LOG_BUFFER_SIZE=${LOG_BUFFER_SIZE})

if(ENABLE_WARNINGS_AS_ERRORS)
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
        target_compile_options(${PROJECT_NAME} PRIVATE /wd4003 /wd4005) #Ignore, Macro warning c4003, c4005 Macro redefinition
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

