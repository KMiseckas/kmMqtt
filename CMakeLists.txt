cmake_minimum_required (VERSION 3.5)

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

if(ENABLE_UNIQUEFUNCTION_SBO)
    target_compile_definitions(${PROJECT_NAME} PUBLIC UNIQUEFUNCTION_SBO_MAX_SIZE=${UNIQUEFUNCTION_SBO_MAX_SIZE})
    target_compile_definitions(${PROJECT_NAME} PUBLIC ENABLE_UNIQUEFUNCTION_SBO)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC UNIQUEFUNCTION_SBO_MAX_SIZE=0)
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

if(ENABLE_ASAN)
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

if(ENABLE_MSAN)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(STATUS "Building with MemorySanitizer")
        set(MSAN_FLAGS "-fsanitize=memory -fsanitize-memory-track-origins -fno-omit-frame-pointer")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MSAN_FLAGS}")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${MSAN_FLAGS}")
    endif()

    if (MSVC)
        message(WARNING "MemorySanitizer is not supported on MSVC.")
    endif()
endif()

if(ENABLE_TSAN)  
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")  
        message(STATUS "Building with ThreadSanitizer")  
        set(TSAN_FLAGS "-fsanitize=thread -fno-omit-frame-pointer")  
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TSAN_FLAGS}")  
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${TSAN_FLAGS}")  
    endif()

    if (MSVC)  
        message(WARNING "ThreadSanitizer is not supported on MSVC.")  
    endif()
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(BUILD_UNIT_TESTS)
    add_subdirectory(tests)
endif()

#Add coverage report generation if coverage is enabled
if(BUILD_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(COVERAGE_LINK_FLAGS --coverage)
        set(COVERAGE_COMPILE_FLAGS -O0 -g --coverage)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(COVERAGE_FLAGS -O0 -g -fprofile-instr-generate -fcoverage-mapping)
    else()
        message(WARNING "Code coverage is only supported with GCC or Clang")
        set(COVERAGE_FLAGS "")
    endif()

    target_compile_options(${PROJECT_NAME} PRIVATE ${COVERAGE_COMPILE_FLAGS})
    target_link_options(${PROJECT_NAME} PRIVATE ${COVERAGE_LINK_FLAGS})

    target_compile_options(cleanMqttTests PRIVATE ${COVERAGE_COMPILE_FLAGS})
    target_link_options(cleanMqttTests PRIVATE ${COVERAGE_LINK_FLAGS})

    set(COVERAGE_OUTPUT_DIR "${CMAKE_BINARY_DIR}/coverage")
            
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # GCC coverage with lcov
        find_program(LCOV_PATH lcov REQUIRED)
        find_program(GENHTML_PATH genhtml REQUIRED)
                
        message(STATUS "Coverage report will be generated with lcov at ${COVERAGE_OUTPUT_DIR}")
                    
        add_custom_target(coverage_report
            COMMAND ${CMAKE_COMMAND} -E echo "Generating coverage report with lcov..."
            COMMAND ${CMAKE_COMMAND} -E remove_directory "${COVERAGE_OUTPUT_DIR}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${COVERAGE_OUTPUT_DIR}"
            COMMAND ${LCOV_PATH} --capture --directory ${CMAKE_BINARY_DIR} --output-file coverage.info
            COMMAND ${LCOV_PATH} --remove coverage.info '*/_deps/*' '/usr/*' '*/tests/*' '*/examples/*' '*/benchmarks/*' --output-file coverage_filtered.info --ignore-errors unused
            COMMAND ${GENHTML_PATH} coverage_filtered.info --output-directory coverage --ignore-errors source
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS cleanMqttTests
            COMMENT "Generating coverage report with lcov"
        )
                
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        # Clang coverage with llvm-cov
        find_program(LLVM_PROFDATA_PATH llvm-profdata)
        find_program(LLVM_COV_PATH llvm-cov)
                
        if(LLVM_PROFDATA_PATH AND LLVM_COV_PATH)
            message(STATUS "Coverage report will be generated with llvm-cov at ${COVERAGE_OUTPUT_DIR}")
                    
            add_custom_target(coverage_report
                COMMAND ${CMAKE_COMMAND} -E echo "Generating coverage report with llvm-cov..."
                COMMAND ${CMAKE_COMMAND} -E remove_directory "${COVERAGE_OUTPUT_DIR}"
                COMMAND ${CMAKE_COMMAND} -E make_directory "${COVERAGE_OUTPUT_DIR}"
                COMMAND ${CMAKE_COMMAND} -E env LLVM_PROFILE_FILE=default.profraw ${CMAKE_CTEST_COMMAND} --output-on-failure
                COMMAND ${LLVM_PROFDATA_PATH} merge -sparse default.profraw -o coverage.profdata
                COMMAND ${LLVM_COV_PATH} show tests/cleanMqttTests -instr-profile=coverage.profdata -format=html -output-dir=${COVERAGE_OUTPUT_DIR} -ignore-filename-regex="(tests|examples|benchmarks|/usr|_deps)"
                COMMAND ${CMAKE_COMMAND} -E echo "Coverage report generated: ${COVERAGE_OUTPUT_DIR}/index.html"
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                DEPENDS cleanMqttTests
                COMMENT "Generating coverage report with llvm-cov"
            )
        else()
            message(WARNING "llvm-profdata and llvm-cov not found. Coverage report will not be generated automatically.")
        endif()
    endif()
endif()

if(BUILD_BENCHMARKING)
    add_subdirectory(benchmarks)
endif()

