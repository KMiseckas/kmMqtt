# Integration Tests

This project includes a broker-backed integration test binary:

- Target: `kmMqttIntegrationTests`
- Location: `integration_tests/`
- Framework: doctest

## Suites And Labels

The integration executable is registered twice via CTest so CI and local runs can choose scope.

| CTest label                | Scope                                                |
| -------------------------- | ---------------------------------------------------- |
| `integration_public_smoke` | Fast public-broker sanity checks                     |
| `integration_public_full`  | Broader behavior and stability validation            |
| `integration`              | Umbrella label for both smoke and full registrations |

## Current Coverage (High Level)

Smoke suites cover:

- WS and WSS connect/disconnect flows
- Basic API lifecycle checks
- Publish/subscribe happy-path scenarios (including QoS 0/1 and invalid-endpoint error surfacing)

Full suites cover:

- Stability loops (repeated connect/disconnect)
- QoS 2 flow validation
- Burst publish flows (QoS 1 and QoS 2)
- Keepalive idle behavior
- Wildcard and multi-topic subscriptions
- Duplicate subscribe and unknown unsubscribe behavior
- SYNC tick end-to-end flow

## Build And Run

```bash
# Configure and build with integration tests enabled
cmake -S . -B build -DBUILD_INTEGRATION_TESTS=ON
cmake --build build

# Smoke-only run
ctest --test-dir build --output-on-failure -L integration_public_smoke

# Full-only run
ctest --test-dir build --output-on-failure -L integration_public_full

# All integration runs
ctest --test-dir build --output-on-failure -L integration
```

## Broker Configuration

Default brokers are configured in `integration_tests/CMakeLists.txt` and compiled into the test binary via definitions in `integration_tests/BrokerConfig.h`.

Default candidate order:

- WS: HiveMQ, EMQX, Mosquitto
- WSS: HiveMQ, EMQX, Mosquitto

Override brokers during configure with cache variables:

```bash
cmake -S . -B build \
  -DBUILD_INTEGRATION_TESTS=ON \
  -DKMMQTT_IT_WS_HOST=localhost \
  -DKMMQTT_IT_WS_PORT=8000 \
  -DKMMQTT_IT_WS_PATH=/mqtt \
  -DKMMQTT_IT_WSS_HOST=localhost \
  -DKMMQTT_IT_WSS_PORT=8884 \
  -DKMMQTT_IT_WSS_PATH=/mqtt \
  -DKMMQTT_IT_TIMEOUT_SEC=15
```

## Notes

- These tests require network access to the configured broker endpoints.
- Sanitizer workflow excludes integration tests (`-LE integration`) to keep sanitizer runs deterministic and faster.
- Enhanced MQTT AUTH flows are not fully supported yet, so integration suites focus on currently supported client behavior.
