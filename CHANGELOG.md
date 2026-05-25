# Changelog

All notable changes to this project will be documented in this file.

The format is based on Keep a Changelog, and this project adheres to Semantic Versioning.

## [1.1.0][unreleased] - 2026-05-23

### Fixed

- [examples] Publish completion status handling for PUBLISH_COMPLETE packet type
- [lib][api] Non-breaking: Topic alias mappings now store an owned topic-name copy, preventing dangling pointers when callers pass temporary or later-mutated C-string buffers.
- [lib] SendQueue now returns SDK send status with out bytes-sent and socket-error values, preventing positive socket error codes from being treated as successful bytes sent.
- [lib] Reconnect socket-connect failure path now branches on the pre-failure state before status mutation, restoring reconnect retry flow when a reconnect attempt fails at socket callback time.
- [lib] Keepalive negotiation now consistently treats broker and client keepalive as seconds and converts once at ping scheduling, fixing mixed seconds/milliseconds behavior.
- [lib] Default client construction now starts async ticking when using default ASYNC options.

### Added

- [tests] SendQueue regression test to verify socket send failures with positive OS error codes are reported as send errors.
- [tests] Reconnect regression test covering socket connect callback failure while reconnecting.
- [tests] Added connect-ack keepalive regression tests covering both fallback and SERVER_KEEP_ALIVE property paths.
- [tests] Added API regression coverage that default `MqttClient` construction starts async ticking.

## [1.0.0]

### Added

- Initial release
