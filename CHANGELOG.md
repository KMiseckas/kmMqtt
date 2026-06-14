# Changelog

All notable changes to this project will be documented in this file.

The format is based on Keep a Changelog, and this project adheres to Semantic Versioning.

## [Unreleased]

### Fixed

- [tests] Integration-test `makeConnectArgs` now appends per-run/per-call uniqueness to client IDs, reducing intermittent public-broker connect flakes caused by client-id collisions across concurrent CI jobs.
- [lib] Publish events now take the payload pointer from the deferred event's moved packet instance, preserving payload access after packet headers moved from heap ownership to inline storage.
- [lib] PUBLISH payload decoding now copies only the remaining MQTT payload bytes instead of the payload buffer capacity, preserving correct received payload sizes when `ByteBuffer` has spare capacity.
- [lib][api] Non-breaking: Packet classes now store concrete variable and payload headers inline instead of heap-allocating header objects, while preserving existing public accessors.

### Added

- [lib][api] Non-breaking: Added minimal `kmStd` thread aliases for project-used threading primitives and `this_thread::sleep_for` so custom thread includes only need to provide the required surface.
- [tests] Updated test waits to use the `kmStd` thread sleep wrapper.
- [examples] Updated example client locking to use the `kmStd` thread aliases.
- [tests] Added broker-free publish event and packet move regression coverage for inline packet header ownership.
- [lib][api] Non-breaking: Added custom allocator foundation with per-client allocator injection through `MqttClientOptions`.
- [lib][api][breaking]: Added `kmMqtt::std` smart-pointer wrapper aliases and helper functions in `kmMqtt/STL/Memory.h` to use SDK allocator and added tests. Some API now accepts `kmMqtt::std` smart pointers instead of std:: versions.
- [doc] Added custom allocator documentation covering defaults, `setAllocator` injection, smart-pointer allocation behavior, allocator flow.
- [doc] Added platform adaptation documentation for custom threading, allocator, logging, and transport seams, and expanded build guidance for `CUSTOM_THREAD_INCLUDE`.

## [1.1.1] - 2026-05-30

### Fixed

- [lib] Memory leak due to not destroyed payload header in publish packet destructor.
- [ci] Post-merge coverage workflow now builds explicit test targets (`kmMqttTests`, `kmMqttIntegrationTests`) instead of the default build target, preventing premature coverage-target execution before tests run.

## [1.1.0] - 2026-05-27

### Fixed

- [examples] Publish completion status handling for PUBLISH_COMPLETE packet type
- [lib][api] Non-breaking: Topic alias mappings now store an owned topic-name copy, preventing dangling pointers when callers pass temporary or later-mutated C-string buffers.
- [lib] SendQueue now returns SDK send status with out bytes-sent and socket-error values, preventing positive socket error codes from being treated as successful bytes sent.
- [lib] SendQueue partial-send metadata cleanup now erases matched entries safely while iterating, preventing stale metadata and incorrect callback dispatch in mixed packet buffers.
- [lib] Reconnect socket-connect failure path now branches on the pre-failure state before status mutation, restoring reconnect retry flow when a reconnect attempt fails at socket callback time.
- [lib] Socket error handling during CONNECTING/RECONNECTING now emits a failed connect event (`Socket_Connect_Failed`) instead of forcing immediate shutdown, improving caller-visible failure reporting.
- [lib] Keepalive negotiation now consistently treats broker and client keepalive as seconds and converts once at ping scheduling, fixing mixed seconds/milliseconds behavior.
- [lib] CONNECT packet composition now guards will-payload encoding when a will payload pointer is absent, preventing null dereference in packet creation.
- [lib] CONNACK receive-maximum parsing now checks the decoded value (not pointer presence), so broker value `0` correctly keeps the protocol default limit.
- [lib] Default client construction now starts async ticking when using default ASYNC options.
- [lib] SessionState message updates now evaluate reordering against pre-update status, restoring expected move-to-end behavior for QoS state transitions.
- [lib][api] Non-breaking: Websocket fallback builds now compile with `BUILD_IXWEBSOCKET=OFF` by keeping `DefaultWebsocket` declarations available in both build modes.
- [lib] Logger compile guards now provide a defensive `LOG_LEVEL` fallback when logs are enabled but no explicit compile definition is supplied.
- [lib][api] Breaking: `ClientErrorCode::TimeOut` and `ClientErrorCode::Using_Tick_Async` now use unique numeric values (`10` and `11`) instead of overlapping with existing codes.
- [lib] Compilation error on Clang/GCC due to move operation on reference in SessionState.
- [lib] Deferred error callbacks now capture owned `ClientError` values instead of `DisconnectArgs` string pointers, fixing ASAN/UBSAN heap-use-after-free during decode/send failure reporting.
- [cmake] Sanitizer linker flags on `kmMqtt` are now set for benchmark and integration test projects.
- [tests][cmake] Unit tests now use doctest `v2.4.12`, pulling in upstream fixes that reduce framework-originated MemorySanitizer noise.

### Added

- [tests] SendQueue regression test to verify socket send failures with positive OS error codes are reported as send errors.
- [tests] Reconnect regression test covering socket connect callback failure while reconnecting.
- [tests] Added connect-ack keepalive regression tests covering both fallback and SERVER_KEEP_ALIVE property paths.
- [tests] Added PacketHelper regression coverage for will packets with null payload pointers.
- [tests] Added receive-maximum regression coverage for broker `RECEIVE_MAXIMUM=0`, asserting default-limit behavior is preserved.
- [tests] Added API regression coverage that default `MqttClient` construction starts async ticking.
- [tests] Added SessionState regression coverage for message reordering on `WaitingForPubRel` transitions.
- [tests] Added logger regression coverage for exception logging and formatted logging API call paths.
- [tests] Added unit and API regression coverage to enforce uniqueness and expected numeric assignments for base `ClientErrorCode` values.
- [tests] Connect API decode-failure tests now assert non-empty error message payload to guard deferred error-message lifetime regressions.
- [tests][cmake] Integration test project (`kmMqttIntegrationTests`) added under `integration_tests/`.
- [ci] Added `dev_ci.yml` GitHub Actions workflow triggering on push/PR to `dev`.
- [ci] Added `release_ci.yml` GitHub Actions workflow triggering on push/PR to `master`.
- [ci] Added `sanitizers.yml` GitHub Actions workflow triggering on push/PR to `master`.
- [ci] Added `post_merge.yml` GitHub Actions workflow triggering on push to `master`.
- [doc] Added README badges.
- [doc] Added dedicated CI workflow documentation in `docs/CI.md`, including trigger/scope summary and quality pipeline flow.
- [doc] Added dedicated integration-test documentation in `docs/INTEGRATION_TESTS.md`, including labels, broker overrides, and local run commands.
- [doc] Updated README feature/protocol notes to clarify that MQTT 5 AUTH/enhanced authentication support is currently partial and not fully supported end-to-end.

## [1.0.0]

### Added

- Initial release
