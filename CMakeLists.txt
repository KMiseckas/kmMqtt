cmake_minimum_required (VERSION 3.14)

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
    target_compile_definitions(${PROJECT_NAME} PUBLIC PUBLIC_API_EXPORT)
endif()

if(FORCE_ADD_PROPERTIES)
    target_compile_definitions(${PROJECT_NAME} PUBLIC FORCE_ADD_PROPERTIES)
endif()

if(ENABLE_LOGS)
    set(LOG_LEVEL_TARGET 0 STRING "Set the log level for which to print logs (TRACE, INFO, DEBUG, WARNING, ERROR).")
    target_compile_definitions(${PROJECT_NAME} PRIVATE 
    LOG_LEVEL=${LOG_LEVEL_TARGET}
    ENABLE_LOGS)
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC LOG_BUFFER_SIZE=${LOG_BUFFER_SIZE})

if(ENABLE_BYTEBUFFER_SBO)
    target_compile_definitions(${PROJECT_NAME} PUBLIC BYTEBUFFER_SBO_MAX_SIZE=${BYTEBUFFER_SBO_MAX_SIZE})
    target_compile_definitions(${PROJECT_NAME} PUBLIC ENABLE_BYTEBUFFER_SBO)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC BYTEBUFFER_SBO_MAX_SIZE=0)
endif()

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

if(ENABLE_UBSAN)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        set(ASAN_FLAGS "-fsanitize=address -fno-omit-frame-pointer")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ASAN_FLAGS}")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ASAN_FLAGS}")
    endif()

    if (MSVC)
        add_compile_options(/fsanitize=address)
        add_link_options(/INCREMENTAL:NO /fsanitize=address)
    endif()
endif()

if(ENABLE_UBSAN)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(STATUS "Building with UndefinedBehaviorSanitizer")
        set(UBSAN_FLAGS "-fsanitize=undefined -fno-sanitize-recover=all")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${UBSAN_FLAGS}")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${UBSAN_FLAGS}")
    endif()

    if (MSVC)
        message(STATUS "Building with /RTC1 for MSVC instead UBSAN.")
        add_compile_options(/RTC1)
    endif()
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(BUILD_UNIT_TESTS)
    add_subdirectory(tests)

    if(ENABLE_UNIT_TESTS)
        enable_testing()
    endif()
endif()

if(BUILD_BENCHMARKING)
    add_subdirectory(benchmarks)
endif()

