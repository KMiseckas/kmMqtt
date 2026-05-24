# Changelog

All notable changes to this project will be documented in this file.

The format is based on Keep a Changelog, and this project adheres to Semantic Versioning.

## [1.1.0][unreleased] - 2026-05-23

### Fixed

- [examples] Publish completion status handling for PUBLISH_COMPLETE packet type
- [lib][api] Non-breaking: Topic alias mappings now store an owned topic-name copy, preventing dangling pointers when callers pass temporary or later-mutated C-string buffers.
- [lib] SendQueue now returns SDK send status with out bytes-sent and socket-error values, preventing positive socket error codes from being treated as successful bytes sent.

### Added

- [tests] SendQueue regression test to verify socket send failures with positive OS error codes are reported as send errors.

## [1.0.0]

### Added

- Initial release
