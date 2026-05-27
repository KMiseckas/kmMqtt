# CI And Quality Workflows

This repository uses four GitHub Actions workflows.

## Workflow Map

```mermaid
flowchart LR
    E[Git Event] --> B{Target}
    B -->|push or PR to dev| D[dev_ci.yml]
    B -->|push or PR to master| R[release_ci.yml]
    B -->|PR to master| S[sanitizers.yml]
    B -->|push to master| P[post_merge.yml]

    D --> D1[Build matrix and unit/API tests]
    D --> D2[Integration smoke on Linux GCC Debug PR]

    R --> R1[Debug and Release matrix builds]
    R --> R2[Unit/API tests]
    R --> R3[Integration smoke and full labels on Linux GCC Debug PR]

    S --> S1[ASan/UBSan and TSan matrix]

    P --> P1[Coverage report generation]
    P --> P2[Doxygen generation]
    P --> P3[GitHub Pages deploy]
```

## Quick Reference

| Workflow         | Trigger             | Purpose                                         |
| ---------------- | ------------------- | ----------------------------------------------- |
| `dev_ci.yml`     | Push/PR on `dev`    | Fast branch validation and cache seeding        |
| `release_ci.yml` | Push/PR on `master` | Release-gate matrix validation                  |
| `sanitizers.yml` | PR on `master`      | Runtime memory/UB/thread diagnostics            |
| `post_merge.yml` | Push on `master`    | Coverage + docs generation and Pages deployment |

## Caching Strategy

- vcpkg directory cache is used for Windows jobs.
- vcpkg binary cache (`VCPKG_BINARY_SOURCES`) is enabled in CI to avoid rebuilding unchanged packages.
- FetchContent `_deps` cache is used across Linux and Windows presets.

## Test Selection In CI

- Unit/API tests run with `-LE integration` in main build workflows.
- Integration tests are label-driven:
  - `integration_public_smoke`
  - `integration_public_full`
  - umbrella label: `integration`

See [INTEGRATION_TESTS.md](INTEGRATION_TESTS.md) for exact commands and broker override options.
