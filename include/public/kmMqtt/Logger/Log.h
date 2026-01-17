#ifndef INCLUDE_KMMQTT_LOGGER_LOG_H
#define INCLUDE_KMMQTT_LOGGER_LOG_H

#include <cstring>
#include <cstdint>
#include <exception>

namespace kmMqtt
{
    void LogTrace(const char* msg, ...) noexcept;
    void LogTrace(const char* category, const char* msg, ...) noexcept;

    void LogInfo(const char* msg, ...) noexcept;
    void LogInfo(const char* category, const char* msg, ...) noexcept;

    void LogDebug(const char* msg, ...) noexcept;
    void LogDebug(const char* category, const char* msg, ...) noexcept;

    void LogWarning(const char* msg, ...) noexcept;
    void LogWarning(const char* category, const char* msg, ...) noexcept;

    void LogError(const char* msg, ...) noexcept;
    void LogError(const char* category, const char* msg, ...) noexcept;

    void LogException(const char* category, const std::exception& e) noexcept(false);
}

#endif //INCLUDE_KMMQTT_LOGGER_LOG_H 
