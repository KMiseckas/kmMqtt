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

set(KMMQTT_IT_TIMEOUT_SEC "15"               CACHE STRING "Seconds to wait for broker responses in each test")

message(STATUS "Integration test WS  broker: ${KMMQTT_IT_WS_SCHEME}://${KMMQTT_IT_WS_HOST}:${KMMQTT_IT_WS_PORT}${KMMQTT_IT_WS_PATH}")
message(STATUS "Integration test WSS broker: ${KMMQTT_IT_WSS_SCHEME}://${KMMQTT_IT_WSS_HOST}:${KMMQTT_IT_WSS_PORT}${KMMQTT_IT_WSS_PATH}")

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
)

# Suppress "enumeration value 'X' not handled in switch" warnings.
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wno-error=switch)
endif()

# Register with CTest using a dedicated label so they can be run or excluded
# independently from unit tests:
#   ctest -L integration          -- run only integration tests
#   ctest -LE integration         -- exclude integration tests
add_test(
    NAME ${PROJECT_NAME}
    COMMAND $<TARGET_FILE:${PROJECT_NAME}>
)
set_tests_properties(${PROJECT_NAME} PROPERTIES LABELS "integration")
