#ifndef INCLUDE_MQTTCLIENT_EVENTS_DIRECTEVENT_H
#define INCLUDE_MQTTCLIENT_EVENTS_DIRECTEVENT_H

#include <functional>
#include <vector>
#include <algorithm>

namespace events
{
    using ListenerHandle = std::size_t;

    template <typename... Args>
    class DirectEvent {
    public:
        using Listener = std::function<void(Args...)>;

        ListenerHandle add(const Listener& listener)
        {
            ++m_nextID;
            m_listeners.emplace_back(m_nextID, listener);
            return m_nextID;
        }

        void remove(ListenerHandle handle)
        {
            if (handle == 0) return;

            m_listeners.erase(
                std::remove_if(m_listeners.begin(), m_listeners.end(),
                    [handle](const auto& pair) { return pair.first == handle; }),
                m_listeners.end());
        }

        void invoke(Args... args)
        {
            for (const auto& pair : m_listeners) 
            {
                pair.second(args...);
            }
        }

    private:
        size_t m_nextID = 0;
        std::vector<std::pair<ListenerHandle, Listener>> m_listeners;
    };
}

#endif //INCLUDE_MQTTCLIENT_EVENTS_DIRECTEVENT_H 
