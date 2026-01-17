// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef MOCKWEBSOCKET_H
#define MOCKWEBSOCKET_H

#include <kmMqtt/Interfaces/IWebSocket.h>
#include <queue>

using namespace kmMqtt;

class MockWebSocket : public IWebSocket
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
    OnConnectCallback onConnectCb;
    OnDisconnectCallback onDisconnectCb;
    OnRecvdCallback onPacketCb;
    OnErrorCallback onErrorCb;

    // Internal state for simulating send/receive
    std::vector<ByteBuffer> sentPackets;
    std::queue<ByteBuffer> pendingResponses;
    bool connected = false;

    bool connect(const mqtt::Address& address) noexcept override
    {
        connectCalled = true;
        lastHost = address.hostname();
        lastPort = address.port();
        connected = connectResult;
        isConnectedResult = connectResult;

        if (connectResult && onConnectCb) 
        {
            onConnectCb(true);
        }

        return connectResult;
    }

    int send(const ByteBuffer& data) noexcept override 
    {
        if (!connected) 
        {
            lastError = 1;

            if (onErrorCb)
            {
                onErrorCb(lastError);
            }

            return -1;
        }

		auto bb{ ByteBuffer(data.size()) };
        bb.append(data.bytes(), data.size());

        sentPackets.push_back(std::move(bb));
        return (int)data.size();
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
    void setOnConnectCallback(OnConnectCallback cb) noexcept override { onConnectCb = cb; }
    void setOnDisconnectCallback(OnDisconnectCallback cb) noexcept override { onDisconnectCb = cb; }
    void setOnRecvdCallback(OnRecvdCallback cb) noexcept override { onPacketCb = cb; }
    void setOnErrorCallback(OnErrorCallback cb) noexcept override { onErrorCb = cb; }

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
