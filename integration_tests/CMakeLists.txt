cmake_minimum_required (VERSION 3.5)

project ("kmMqttIntegrationTests" LANGUAGES CXX)

# ---------------------------------------------------------------------------
# Broker configuration
# Override these via -D<VAR>=... on the cmake command line to point at a
# different broker (e.g. a local Mosquitto container in a CI pipeline).
#
# WS  (plain WebSocket)  — HiveMQ public broker defaults: ws://broker.hivemq.com:8000/mqtt
# WSS (TLS  WebSocket)   — HiveMQ public broker defaults: wss://broker.hivemq.com:8884/mqtt
#
# To target a local broker (e.g. Mosquitto in Docker):
#   cmake -DKMMQTT_IT_WS_HOST=localhost  -DKMMQTT_IT_WS_PORT=8000  \
#         -DKMMQTT_IT_WSS_HOST=localhost -DKMMQTT_IT_WSS_PORT=8884 ...
# ---------------------------------------------------------------------------

# WS endpoint
set(KMMQTT_IT_WS_SCHEME  "ws"                CACHE STRING "Scheme for plain-WebSocket integration-test broker")
set(KMMQTT_IT_WS_HOST    "broker.hivemq.com"  CACHE STRING "Hostname for plain-WebSocket integration-test broker")
set(KMMQTT_IT_WS_PORT    "8000"               CACHE STRING "Port for plain-WebSocket integration-test broker")
set(KMMQTT_IT_WS_PATH    "/mqtt"              CACHE STRING "WebSocket path for plain-WebSocket integration-test broker")

# WSS endpoint
set(KMMQTT_IT_WSS_SCHEME "wss"               CACHE STRING "Scheme for TLS-WebSocket integration-test broker")
set(KMMQTT_IT_WSS_HOST   "broker.hivemq.com"  CACHE STRING "Hostname for TLS-WebSocket integration-test broker")
set(KMMQTT_IT_WSS_PORT   "8884"              CACHE STRING "Port for TLS-WebSocket integration-test broker")
set(KMMQTT_IT_WSS_PATH   "/mqtt"             CACHE STRING "WebSocket path for TLS-WebSocket integration-test broker")

# WS fallback endpoints
set(KMMQTT_IT_WS2_HOST   "broker.emqx.io"    CACHE STRING "Hostname for secondary plain-WebSocket integration-test broker")
set(KMMQTT_IT_WS2_PORT   "8083"              CACHE STRING "Port for secondary plain-WebSocket integration-test broker")
set(KMMQTT_IT_WS2_PATH   "/mqtt"             CACHE STRING "WebSocket path for secondary plain-WebSocket integration-test broker")

set(KMMQTT_IT_WS3_HOST   "test.mosquitto.org" CACHE STRING "Hostname for tertiary plain-WebSocket integration-test broker")
set(KMMQTT_IT_WS3_PORT   "8080"               CACHE STRING "Port for tertiary plain-WebSocket integration-test broker")
set(KMMQTT_IT_WS3_PATH   "/mqtt"              CACHE STRING "WebSocket path for tertiary plain-WebSocket integration-test broker")

# WSS fallback endpoints
set(KMMQTT_IT_WSS2_HOST  "broker.emqx.io"    CACHE STRING "Hostname for secondary TLS-WebSocket integration-test broker")
set(KMMQTT_IT_WSS2_PORT  "8084"              CACHE STRING "Port for secondary TLS-WebSocket integration-test broker")
set(KMMQTT_IT_WSS2_PATH  "/mqtt"             CACHE STRING "WebSocket path for secondary TLS-WebSocket integration-test broker")

set(KMMQTT_IT_WSS3_HOST  "test.mosquitto.org" CACHE STRING "Hostname for tertiary TLS-WebSocket integration-test broker")
set(KMMQTT_IT_WSS3_PORT  "8081"               CACHE STRING "Port for tertiary TLS-WebSocket integration-test broker")
set(KMMQTT_IT_WSS3_PATH  "/mqtt"              CACHE STRING "WebSocket path for tertiary TLS-WebSocket integration-test broker")

set(KMMQTT_IT_TIMEOUT_SEC "15"               CACHE STRING "Seconds to wait for broker responses in each test")

message(STATUS "Integration test WS  broker: ${KMMQTT_IT_WS_SCHEME}://${KMMQTT_IT_WS_HOST}:${KMMQTT_IT_WS_PORT}${KMMQTT_IT_WS_PATH}")
message(STATUS "Integration test WSS broker: ${KMMQTT_IT_WSS_SCHEME}://${KMMQTT_IT_WSS_HOST}:${KMMQTT_IT_WSS_PORT}${KMMQTT_IT_WSS_PATH}")
message(STATUS "Integration test WS  fallback #2: ${KMMQTT_IT_WS_SCHEME}://${KMMQTT_IT_WS2_HOST}:${KMMQTT_IT_WS2_PORT}${KMMQTT_IT_WS2_PATH}")
message(STATUS "Integration test WS  fallback #3: ${KMMQTT_IT_WS_SCHEME}://${KMMQTT_IT_WS3_HOST}:${KMMQTT_IT_WS3_PORT}${KMMQTT_IT_WS3_PATH}")
message(STATUS "Integration test WSS fallback #2: ${KMMQTT_IT_WSS_SCHEME}://${KMMQTT_IT_WSS2_HOST}:${KMMQTT_IT_WSS2_PORT}${KMMQTT_IT_WSS2_PATH}")
message(STATUS "Integration test WSS fallback #3: ${KMMQTT_IT_WSS_SCHEME}://${KMMQTT_IT_WSS3_HOST}:${KMMQTT_IT_WSS3_PORT}${KMMQTT_IT_WSS3_PATH}")

file(GLOB_RECURSE integration_test_source_files "*.cpp" "*.h")
add_executable(${PROJECT_NAME} ${integration_test_source_files})

include(FetchContent)
FetchContent_Declare(
    Doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG ae7a13539fb71f270b87eb2e874fbac80bc8dda2 #v2.4.11
)
FetchContent_MakeAvailable(Doctest)

target_include_directories(${PROJECT_NAME} PRIVATE ${doctest_SOURCE_DIR}/doctest)
target_link_libraries(${PROJECT_NAME} PRIVATE kmMqtt)

# Expose broker config to the test binary as compile-time definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
    KMMQTT_IT_WS_SCHEME="${KMMQTT_IT_WS_SCHEME}"
    KMMQTT_IT_WS_HOST="${KMMQTT_IT_WS_HOST}"
    KMMQTT_IT_WS_PORT="${KMMQTT_IT_WS_PORT}"
    KMMQTT_IT_WS_PATH="${KMMQTT_IT_WS_PATH}"
    KMMQTT_IT_WSS_SCHEME="${KMMQTT_IT_WSS_SCHEME}"
    KMMQTT_IT_WSS_HOST="${KMMQTT_IT_WSS_HOST}"
    KMMQTT_IT_WSS_PORT="${KMMQTT_IT_WSS_PORT}"
    KMMQTT_IT_WSS_PATH="${KMMQTT_IT_WSS_PATH}"
    KMMQTT_IT_TIMEOUT_SEC=${KMMQTT_IT_TIMEOUT_SEC}
    KMMQTT_IT_WS2_HOST="${KMMQTT_IT_WS2_HOST}"
    KMMQTT_IT_WS2_PORT="${KMMQTT_IT_WS2_PORT}"
    KMMQTT_IT_WS2_PATH="${KMMQTT_IT_WS2_PATH}"
    KMMQTT_IT_WS3_HOST="${KMMQTT_IT_WS3_HOST}"
    KMMQTT_IT_WS3_PORT="${KMMQTT_IT_WS3_PORT}"
    KMMQTT_IT_WS3_PATH="${KMMQTT_IT_WS3_PATH}"
    KMMQTT_IT_WSS2_HOST="${KMMQTT_IT_WSS2_HOST}"
    KMMQTT_IT_WSS2_PORT="${KMMQTT_IT_WSS2_PORT}"
    KMMQTT_IT_WSS2_PATH="${KMMQTT_IT_WSS2_PATH}"
    KMMQTT_IT_WSS3_HOST="${KMMQTT_IT_WSS3_HOST}"
    KMMQTT_IT_WSS3_PORT="${KMMQTT_IT_WSS3_PORT}"
    KMMQTT_IT_WSS3_PATH="${KMMQTT_IT_WSS3_PATH}"
)

# Include standard C++ headers (originally added to fix warnings due to specilizations of std::tuple)
target_compile_definitions(${PROJECT_NAME} PRIVATE
    DOCTEST_CONFIG_USE_STD_HEADERS
)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
	target_compile_options(${PROJECT_NAME} PRIVATE -Wno-error=switch)
endif()

target_link_options(${PROJECT_NAME} PRIVATE ${KMMQTT_SANITIZER_FLAGS})
set_target_properties(${PROJECT_NAME} PROPERTIES ENVIRONMENT "MSAN_OPTIONS=halt_on_error=false")

# Register smoke and full integration test runs separately so CI can choose
# quick checks for PRs and broader checks on protected branch flows.
#   ctest -L integration_public_smoke
#   ctest -L integration_public_full
add_test(
    NAME ${PROJECT_NAME}_smoke
    COMMAND $<TARGET_FILE:${PROJECT_NAME}> "--test-suite=Integration - Smoke*"
)
set_tests_properties(${PROJECT_NAME}_smoke PROPERTIES LABELS "integration;integration_public_smoke")

add_test(
    NAME ${PROJECT_NAME}_full
    COMMAND $<TARGET_FILE:${PROJECT_NAME}> "--test-suite=Integration - Full*"
)
set_tests_properties(${PROJECT_NAME}_full PROPERTIES LABELS "integration;integration_public_full")
