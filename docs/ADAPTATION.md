# Platform Adaptation

kmMqtt is designed so the MQTT protocol core can stay portable while platform-facing concerns are replaced in small, focused seams.

## Current adaptation seams

| Area | Primary hook | Typical use |
| --- | --- | --- |
| Thread primitives | `kmMqtt/STL/KmThread.h` | Map SDK thread usage to a platform thread implementation |
| Chrono primitives | `kmMqtt/STL/KmChrono.h` | Map SDK clock and duration usage to a platform time implementation |
| Memory | `IAllocator`, `setAllocator()`, `kmMqtt::kmStd` | Route SDK-owned allocations into custom heaps, pools, or tracking systems |
| Logging | `ILogger`, `setLogger()` | Forward SDK logs into an engine logger, telemetry pipeline, or platform console |
| Transport / environment | `IMqttEnvironment`, `IWebSocket` | Replace the default socket/runtime layer for custom or closed-source platforms |

## Threading

`include/public/kmMqtt/STL/KmThread.h` is the SDK wrapper for threading primitives used by kmMqtt internals and public headers. By default it aliases the standard library.

To replace it:

1. Create a header that defines the `kmMqtt::kmStd` thread surface used by the SDK.
2. Make that header available on the compiler include path for both the kmMqtt build and any consuming code that includes kmMqtt public headers.
3. Define `CUSTOM_THREAD_INCLUDE` to that header path before kmMqtt is compiled.

The current contract expected from the custom header is:

- `kmMqtt::kmStd::thread`
- `kmMqtt::kmStd::mutex`
- `kmMqtt::kmStd::condition_variable`
- `kmMqtt::kmStd::atomic<T>`
- `kmMqtt::kmStd::lock_guard<Mutex>`
- `kmMqtt::kmStd::unique_lock<Mutex>`
- `kmMqtt::kmStd::this_thread::sleep_for(...)`
- `kmMqtt::kmStd::make_thread(...)`

Preferred CMake integration:

```cmake
add_compile_definitions(
  CUSTOM_THREAD_INCLUDE=\"platform/KmThreadPlatform.h\"
)
```

Or, when the `kmMqtt` target is already available:

```cmake
target_compile_definitions(kmMqtt PUBLIC
  CUSTOM_THREAD_INCLUDE=\"platform/KmThreadPlatform.h\"
)
```

Compiler-definition form:

```text
GCC / Clang: -DCUSTOM_THREAD_INCLUDE=\"platform/KmThreadPlatform.h\"
MSVC: /DCUSTOM_THREAD_INCLUDE=\"platform/KmThreadPlatform.h\"
```

## Chrono

`include/public/kmMqtt/STL/KmChrono.h` is the SDK wrapper for clock and duration primitives used by kmMqtt internals and public headers. By default it aliases `std::chrono` as `kmMqtt::kmStd::chrono`.

To replace it:

1. Create a header that defines the `kmMqtt::kmStd::chrono` surface used by the SDK.
2. Make that header available on the compiler include path for both the kmMqtt build and any consuming code that includes kmMqtt public headers.
3. Define `CUSTOM_CHRONO_INCLUDE` to that header path before kmMqtt is compiled.

The current contract expected from the custom header is:

- `kmMqtt::kmStd::chrono::steady_clock`
- `kmMqtt::kmStd::chrono::steady_clock::time_point`
- `kmMqtt::kmStd::chrono::seconds`
- `kmMqtt::kmStd::chrono::milliseconds`
- `kmMqtt::kmStd::chrono::duration_cast<Duration>(...)`

Preferred CMake integration:

```cmake
add_compile_definitions(
  CUSTOM_CHRONO_INCLUDE=\"platform/KmChronoPlatform.h\"
)
```

Or, when the `kmMqtt` target is already available:

```cmake
target_compile_definitions(kmMqtt PUBLIC
  CUSTOM_CHRONO_INCLUDE=\"platform/KmChronoPlatform.h\"
)
```

Compiler-definition form:

```text
GCC / Clang: -DCUSTOM_CHRONO_INCLUDE=\"platform/KmChronoPlatform.h\"
MSVC: /DCUSTOM_CHRONO_INCLUDE=\"platform/KmChronoPlatform.h\"
```

The wrappers exist for the SDK seam. Application code can still use `std::thread`, `std::this_thread`, and `std::chrono` directly if that fits the host project better.

## Memory

kmMqtt routes SDK-owned allocations through `IAllocator` and `setAllocator()`.

- Use this when the host project has a custom heap, pool allocator, or memory tracking system.
- Where a standard-library type supports allocator injection, kmMqtt uses an allocator-aware wrapper under `kmMqtt::kmStd` so SDK-owned standard-type allocations follow the currently installed allocator.
- `kmMqtt::kmStd::make_shared` and `kmMqtt::kmStd::make_unique` follow the currently installed allocator.

The full allocator guide is in [MEMORY.md](MEMORY.md).

## Logging

kmMqtt logging is replaceable through `ILogger` and `setLogger()`.

- If no logger has been installed when a client is created, kmMqtt falls back to `DefaultLogger`.
- Install a custom logger before creating clients when you want SDK logs to go through an engine logger or platform logging service.

Basic pattern:

```cpp
#include <kmMqtt/Logger/LoggerInstance.h>
#include <kmMqtt/Interfaces/ILogger.h>

class EngineLogger : public kmMqtt::ILogger
{
public:
    void Log(kmMqtt::LogLevel level, const char* msg) const noexcept override;
    void Log(kmMqtt::LogLevel level, const char* category, const char* msg) const noexcept override;
};

EngineLogger logger;
kmMqtt::setLogger(&logger);
```

## Transport and environment

The protocol layer is separated from the runtime transport layer through `IMqttEnvironment` and `IWebSocket`.

- `IMqttEnvironment` creates the runtime `Config` and an `IWebSocket`.
- `IWebSocket` owns the platform socket behavior: connect, send, close, tick, status queries, and network callbacks.
- When `BUILD_IXWEBSOCKET=ON`, kmMqtt includes the default IXWebSocket-backed transport.
- When `BUILD_IXWEBSOCKET=OFF`, you can still compile the SDK and provide your own environment/transport implementation.
