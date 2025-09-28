#ifndef MOCKWEBSOCKET_H
#define MOCKWEBSOCKET_H

#include <cleanMqtt/Interfaces/IWebSocket.h>
#include <queue>

using namespace cleanMqtt;
using namespace cleanMqtt::mqtt;

class MockWebSocket : public interfaces::IWebSocket
{
public:
    bool connectCalled = false;
    std::string lastHost, lastPort;
    bool connectResult = true;
    bool closeCalled = false;
    bool isConnectedResult = false;
    int lastError = 0;
    int lastCloseCode = 0;
    std::string lastCloseReason = "none";
    interfaces::OnConnectCallback onConnectCb;
    interfaces::OnDisconnectCallback onDisconnectCb;
    interfaces::OnPacketRecvdCallback onPacketCb;
    interfaces::OnErrorCallback onErrorCb;

    // Internal state for simulating send/receive
    std::vector<ByteBuffer> sentPackets;
    std::queue<ByteBuffer> pendingResponses;
    bool connected = false;

    bool connect(const std::string& hostname, const std::string& port = "80") noexcept override 
    {
        connectCalled = true;
        lastHost = hostname;
        lastPort = port;
        connected = connectResult;
        isConnectedResult = connectResult;

        if (connectResult && onConnectCb) 
        {
            onConnectCb(true);
        }

        return connectResult;
    }

    bool send(const ByteBuffer& data) noexcept override 
    {
        if (!connected) 
        {
            lastError = 1;

            if (onErrorCb)
            {
                onErrorCb(lastError);
            }

            return false;
        }

		auto bb{ ByteBuffer(data.size()) };
        bb.append(data.bytes(), data.size());

        sentPackets.push_back(std::move(bb));
        return true;
    }

    bool close() noexcept override 
    {
        closeCalled = true;

        if (connected) 
        {
            connected = false;
            isConnectedResult = false;
            if (onDisconnectCb) onDisconnectCb();
        }

        return true;
    }

    void tick() noexcept override 
    {
        // Simulate receiving a packet if one is queued
        if (connected && !pendingResponses.empty()) 
        {
            ByteBuffer resp = std::move(pendingResponses.front());
            pendingResponses.pop();
            if (onPacketCb)
            {
                onPacketCb(std::move(resp));
            }
        }
    }

    bool isConnected() const noexcept override { return isConnectedResult; }
    int getLastError() const noexcept override { return lastError; }
    int getLastCloseCode() const noexcept override { return lastCloseCode; }
    const char* getLastCloseReason() const noexcept override { return lastCloseReason.c_str(); }
    void setOnConnectCallback(interfaces::OnConnectCallback cb) noexcept override { onConnectCb = cb; }
    void setOnDisconnectCallback(interfaces::OnDisconnectCallback cb) noexcept override { onDisconnectCb = cb; }
    void setOnPacketRecvdCallback(interfaces::OnPacketRecvdCallback cb) noexcept override { onPacketCb = cb; }
    void setOnErrorCallback(interfaces::OnErrorCallback cb) noexcept override { onErrorCb = cb; }

    // Test helper: queue a response to be "received" on next tick
    void queueMockResponse(const ByteBuffer& data) 
    {
        auto bb{ ByteBuffer(data.size()) };
        bb.append(data.bytes(), data.size());


        pendingResponses.push(std::move(bb));
    }

    // Test helper: reset all state
    void reset() 
    {
        connectCalled = false;
        lastHost.clear();
        lastPort.clear();
        connectResult = true;
        closeCalled = false;
        isConnectedResult = false;
        lastError = 0;
        lastCloseCode = 0;
        lastCloseReason = "none";
        sentPackets.clear();
        while (!pendingResponses.empty()) pendingResponses.pop();
        connected = false;
    }
};

#endif // MOCKWEBSOCKET_H
