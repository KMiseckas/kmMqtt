# Memory and Allocators

kmMqtt routes SDK-owned allocations through a small allocator abstraction so applications can plug in their own memory tracking, pooling, or platform allocator.

Where the standard library type supports allocator injection, kmMqtt also exposes allocator-aware wrappers under `kmMqtt::kmStd` and uses those wrappers in SDK-owned code paths. Those wrappers default to the allocator currently selected through `kmMqtt::setAllocator()`.

## Default allocator

By default, kmMqtt uses `kmMqtt::DefaultAllocator`. The default allocator is owned by the SDK allocator context in `AllocatorContext.cpp`, and it is selected automatically when no custom allocator has been assigned.

Passing `nullptr` to `kmMqtt::setAllocator()` resets the SDK back to `DefaultAllocator`.

## Custom allocator API

Implement `kmMqtt::IAllocator` and install it with `kmMqtt::setAllocator(IAllocator*)` before constructing SDK objects that should use it.

```cpp
#include <kmMqtt/Interfaces/IAllocator.h>
#include <kmMqtt/Memory/AllocatorContext.h>
#include <kmMqtt/MqttClient.h>

class GameAllocator : public kmMqtt::IAllocator
{
public:
    void* allocate(std::size_t size, std::size_t alignment) override
    {
        return allocateFromGameHeap(size, alignment);
    }

    void deallocate(void* ptr, std::size_t size, std::size_t alignment) noexcept override
    {
        deallocateFromGameHeap(ptr, size, alignment);
    }
};

GameAllocator allocator;
kmMqtt::setAllocator(&allocator);

kmMqtt::mqtt::MqttClient client;

// Optional: reset to the SDK default allocator.
kmMqtt::setAllocator(nullptr);
```

The installed allocator must outlive all SDK objects and smart pointers that may allocate or deallocate through it.

## Allocation helpers

The allocator context is used by the SDK allocation helpers:

- `kmNew(Type, ...)` and `kmDelete(ptr)` use the currently installed allocator.
- `kmNewWith(allocator, Type, ...)` and `kmDeleteWith(allocator, ptr)` use an explicit allocator reference.
- `kmMqtt::StdAllocator<T>` adapts `IAllocator` to standard allocator-aware containers.

## `kmMqtt::kmStd` types

For standard-library types that support allocator injection, kmMqtt provides allocator-aware wrappers under the `kmMqtt::kmStd` namespace.

- Examples include `kmMqtt::kmStd::string`, `vector`, `map`, `unordered_map`, `set`, `list`, `deque`, `queue`, and related wrappers in `include/public/kmMqtt/STL/`.
- These wrappers default to `kmMqtt::StdAllocator<T>`, which in turn uses the allocator currently selected by `kmMqtt::setAllocator()`.
- The goal is that SDK-owned standard-type allocations follow the same allocator context as other SDK-owned memory, where the underlying standard-library type makes that possible.

Not every standard-library facility supports custom allocators. In those cases kmMqtt cannot force allocator usage through `kmMqtt::setAllocator()`, so the allocator guarantee is intentionally phrased as "where possible."

## Smart pointers

kmMqtt also provides allocator-aware smart pointer helpers in `kmMqtt/STL/KmMemory.h`:

- `kmMqtt::kmStd::make_shared<T>(...)` uses `std::allocate_shared` with `kmMqtt::StdAllocator<T>`.
- `kmMqtt::kmStd::make_unique<T>(...)` allocates object storage through the current allocator and destroys it with `KmDeleter`.
- `kmMqtt::kmStd::shared_ptr<T>`, `weak_ptr<T>`, and `unique_ptr<T>` are the SDK-facing pointer aliases used by public and internal APIs.

Because these helpers use `kmMqtt::getAllocator()`, SDK-owned smart pointer allocations follow the same allocator selected with `kmMqtt::setAllocator()`.
