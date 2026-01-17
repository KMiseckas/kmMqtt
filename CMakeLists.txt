cmake_minimum_required (VERSION 3.5)

# vcpkg integration (optional, only if VCPKG_ROOT is set or vcpkg toolchain is used)
# This allows vcpkg to provide dependencies like OpenSSL on desktop platforms
# Console platforms and custom toolchains should provide OpenSSL via their own toolchain files
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
    message(STATUS "Using vcpkg toolchain from VCPKG_ROOT: $ENV{VCPKG_ROOT}")
endif()

include(cmake/options.cmake)

project (
    "kmMqtt" 
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

# IXWebSocket integration (Default websocket implementation)
if(BUILD_IXWEBSOCKET)
    message(STATUS "Fetching IXWebSocket library...")
    include(FetchContent)
    
    # Find OpenSSL using standard CMake mechanisms
    # OpenSSL can be provided by:
    #   - vcpkg (desktop platforms: Windows/Linux/macOS)
    #   - System package manager (Linux: libssl-dev, macOS: brew)
    #   - Platform SDK (console platforms)
    #   - Custom toolchain file (CMAKE_TOOLCHAIN_FILE)
    #   - Manual installation (OPENSSL_ROOT_DIR or CMAKE_PREFIX_PATH)
    #
    # Desktop setup instructions:
    #   - vcpkg (recommended for Windows):
    #       1. Install vcpkg: https://github.com/microsoft/vcpkg
    #       2. Set VCPKG_ROOT environment variable
    #       3. vcpkg install openssl (automatic after cmake configuration via vcpkg manifest mode)
    #       OR use: -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake
    #
    #   - Linux: sudo apt-get install libssl-dev (Debian/Ubuntu)
    #            sudo yum install openssl-devel (RHEL/CentOS/Fedora)
    #
    #   - macOS: brew install openssl
    #            May need: -DCMAKE_PREFIX_PATH="$(brew --prefix openssl)"
    #
    # Console platforms:
    #   - Provide OpenSSL via your platform's toolchain file
    #   - Set OPENSSL_ROOT_DIR to your platform's OpenSSL installation
    #
    # For more information: https://cmake.org/cmake/help/latest/module/FindOpenSSL.html
    
    find_package(OpenSSL REQUIRED)
    
    if(NOT OpenSSL_FOUND)
        message(FATAL_ERROR 
            "OpenSSL not found!\n"
            "IXWebSocket requires OpenSSL for WSS (secure WebSocket) support.\n"
            "\n"
            "Desktop platforms:\n"
            "  - Windows: Use vcpkg (set VCPKG_ROOT env var) or manual installation\n"
            "  - Linux: Install libssl-dev via your package manager\n"
            "  - macOS: Install via Homebrew (brew install openssl)\n"
            "\n"
            "Console platforms:\n"
            "  - Provide OpenSSL via your platform's toolchain file\n"
            "  - Set OPENSSL_ROOT_DIR or use CMAKE_TOOLCHAIN_FILE\n"
            "\n"
            "See CMakeLists.txt comments for detailed instructions."
        )
    endif()
    
    message(STATUS "Found OpenSSL ${OPENSSL_VERSION}: ${OPENSSL_INCLUDE_DIR}")
    
    # Enable TLS and OpenSSL support in IXWebSocket
    set(USE_TLS ON CACHE BOOL "Enable TLS support in IXWebSocket" FORCE)
    set(USE_OPEN_SSL ON CACHE BOOL "Enable OpenSSL in IXWebSocket" FORCE)
    
    # Disable zlib compression (not required for TLS, can be enabled separately if needed)
    set(USE_ZLIB OFF CACHE BOOL "Disable zlib compression in IXWebSocket" FORCE)
    
    FetchContent_Declare(
        ixwebsocket
        GIT_REPOSITORY https://github.com/machinezone/IXWebSocket.git
        GIT_TAG v11.4.5
    )
    FetchContent_MakeAvailable(ixwebsocket)
    
    # Link using standard CMake targets (works with vcpkg, system, and custom OpenSSL)
    target_link_libraries(${PROJECT_NAME} PUBLIC ixwebsocket)
    target_compile_definitions(${PROJECT_NAME} PUBLIC BUILD_IXWEBSOCKET)
    message(STATUS "IXWebSocket library integrated successfully with TLS/OpenSSL support")
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
        set(ASAN_FLAGS "-fsanitize=address" "-fno-omit-frame-pointer")
        target_compile_options(${PROJECT_NAME} PRIVATE ${ASAN_FLAGS})
        target_link_options(${PROJECT_NAME} PRIVATE ${ASAN_FLAGS})
    endif()

    if (MSVC)
        add_compile_options(/fsanitize=address)
        add_link_options(/INCREMENTAL:NO /fsanitize=address)
    endif()
endif()

if(ENABLE_UBSAN)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(STATUS "Building with UndefinedBehaviorSanitizer")
        set(UBSAN_FLAGS "-fsanitize=undefined" "-fno-sanitize-recover=all")
        target_compile_options(${PROJECT_NAME} PRIVATE ${UBSAN_FLAGS})
        target_link_options(${PROJECT_NAME} PRIVATE ${UBSAN_FLAGS})
    endif()

    if (MSVC)
        message(STATUS "Building with /RTC1 for MSVC instead UBSAN.")
        add_compile_options(/RTC1)
    endif()
endif()

if(ENABLE_MSAN)
    if(NOT ENABLE_ASAN AND NOT ENABLE_TSAN)
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            message(STATUS "Building with MemorySanitizer")
            set(MSAN_FLAGS "-fsanitize=memory" "-fsanitize-memory-track-origins" "-fno-omit-frame-pointer" "-fsanitize-recover=all")
            target_compile_options(${PROJECT_NAME} PRIVATE ${MSAN_FLAGS})
            target_link_options(${PROJECT_NAME} PRIVATE ${MSAN_FLAGS})
        endif()

        if (MSVC)
            message(FATAL_ERROR "MemorySanitizer is not supported on MSVC.")
        endif()
    else()
        message(FATAL_ERROR "MemorySanitizer cannot be enabled together with AddressSanitizer or ThreadSanitizer")
    endif()
endif()

if(ENABLE_TSAN)  
    if(NOT ENABLE_ASAN AND NOT ENABLE_MSAN AND NOT ENABLE_UBSAN)
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")  
            message(STATUS "Building with ThreadSanitizer")  
            set(TSAN_FLAGS "-fsanitize=thread" "-fno-omit-frame-pointer")  
            target_compile_options(${PROJECT_NAME} PRIVATE ${TSAN_FLAGS})
            target_link_options(${PROJECT_NAME} PRIVATE ${TSAN_FLAGS})
        endif()

        if (MSVC)  
            message(FATAL_ERROR "ThreadSanitizer is not supported on MSVC.")  
        endif()
    else()
        message(FATAL_ERROR "ThreadSanitizer cannot be enabled together with AddressSanitizer or MemorySanitizer")
    endif()
endif()

set(KMMQTT_SANITIZER_FLAGS ${MSAN_FLAGS} ${ASAN_FLAGS} ${UBSAN_FLAGS} ${TSAN_FLAGS})

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(BUILD_UNIT_TESTS)
    set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "Minimum CMake version for unit tests")
    add_subdirectory(tests)
endif()

#Add coverage report generation if coverage is enabled
if(BUILD_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(COVERAGE_LINK_FLAGS --coverage)
        set(COVERAGE_COMPILE_FLAGS -O0 -g --coverage)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(COVERAGE_COMPILE_FLAGS "-O0" "-g" "-fprofile-instr-generate" "-fcoverage-mapping")
        set(COVERAGE_LINK_FLAGS "-fprofile-instr-generate")
    else()
        message(WARNING "Code coverage is only supported with GCC or Clang")
       set(COVERAGE_COMPILE_FLAGS "")
        set(COVERAGE_LINK_FLAGS "")
    endif()

    target_compile_options(${PROJECT_NAME} PRIVATE ${COVERAGE_COMPILE_FLAGS})
    target_compile_options(kmMqttTests PRIVATE ${COVERAGE_COMPILE_FLAGS})

    target_link_options(${PROJECT_NAME} PRIVATE ${COVERAGE_LINK_FLAGS})
    target_link_options(kmMqttTests PRIVATE ${COVERAGE_LINK_FLAGS})
    target_link_options(mqttClient PRIVATE ${COVERAGE_LINK_FLAGS})

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
            DEPENDS kmMqttTests
            COMMENT "Generating coverage report with lcov"
        )
                
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        # Clang coverage with llvm-cov
        find_program(LLVM_PROFDATA_PATH llvm-profdata)
        find_program(LLVM_COV_PATH llvm-cov)
                
        if(LLVM_PROFDATA_PATH AND LLVM_COV_PATH)
            message(STATUS "Coverage report will be generated with llvm-cov at ${COVERAGE_OUTPUT_DIR}")
                    
            enable_testing()
                add_test(
                    NAME kmMqttTests 
                    COMMAND $<TARGET_FILE:kmMqttTests>
                )

            set(ENV{LLVM_PROFILE_FILE} "default.profraw")

            add_custom_target(coverage_report
                COMMAND ${CMAKE_COMMAND} -E echo "Generating coverage report with llvm-cov..."
                COMMAND ${CMAKE_COMMAND} -E remove_directory "${COVERAGE_OUTPUT_DIR}"
                COMMAND ${CMAKE_COMMAND} -E make_directory "${COVERAGE_OUTPUT_DIR}"
                COMMAND ${CMAKE_COMMAND} -E env LLVM_PROFILE_FILE=default.profraw ${CMAKE_CTEST_COMMAND} --output-on-failure
                COMMAND ${LLVM_PROFDATA_PATH} merge -sparse default.profraw -o coverage.profdata
                COMMAND ${LLVM_COV_PATH} show $<TARGET_FILE:kmMqttTests> -instr-profile=coverage.profdata -format=html -output-dir=${COVERAGE_OUTPUT_DIR} -ignore-filename-regex="tests|examples|benchmarks|/usr|_deps"
                COMMAND ${CMAKE_COMMAND} -E echo "Coverage report generated: ${COVERAGE_OUTPUT_DIR}/index.html"
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                DEPENDS kmMqttTests
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

#Doxygen documentation generation
if(BUILD_DOCS)
    find_package(Doxygen OPTIONAL_COMPONENTS dot)
    if(DOXYGEN_FOUND)
        #Configure the Doxyfile
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
        
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        
        #Add a custom target to build documentation
        add_custom_target(docs
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
        )

        install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/html
            DESTINATION share/doc/${PROJECT_NAME}
            OPTIONAL)
        
        message(STATUS "Doxygen found. Use 'cmake --build . --target docs' to generate documentation.")
    else()
        message(WARNING "BUILD_DOCS enabled but Doxygen not found. Documentation will not be available.")
    endif()
endif()

