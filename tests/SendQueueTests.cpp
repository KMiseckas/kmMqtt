// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <doctest.h>
#include <kmMqtt/Mqtt/Transport/SendQueue.h>

namespace
{
	using namespace kmMqtt;
	using namespace kmMqtt::mqtt;

	class MockSendSocket : public IWebSocket
	{
	public:
		bool connect(const Address&) noexcept override { return true; }
		int send(const ByteBuffer&) noexcept override
		{
			++sendCalls;
			return sendReturnValue;
		}
		bool close() noexcept override { return true; }
		void tick() noexcept override {}
		bool isConnected() const noexcept override { return true; }
		int getLastError() const noexcept override { return lastErrorValue; }
		int getLastCloseCode() const noexcept override { return 0; }
		const char* getLastCloseReason() const noexcept override { return ""; }
		void setOnConnectCallback(OnConnectCallback) noexcept override {}
		void setOnDisconnectCallback(OnDisconnectCallback) noexcept override {}
		void setOnRecvdCallback(OnRecvdCallback) noexcept override {}
		void setOnErrorCallback(OnErrorCallback) noexcept override {}

		int sendReturnValue{ -1 };
		int lastErrorValue{ 10054 };
		int sendCalls{ 0 };
	};

	class MockPacketComposer : public IPacketComposer
	{
	public:
		MockPacketComposer() noexcept
			: IPacketComposer(nullptr)
		{
		}

		ComposeResult compose() noexcept override
		{
			EncodeResult result{ EncodeErrorCode::NO_ERROR };
			result.packetType = PacketType::CONNECT;

			ByteBuffer data{ 2 };
			data += static_cast<std::uint8_t>(0x10);
			data += static_cast<std::uint8_t>(0x00);
			return ComposeResult{ result, std::move(data) };
		}

		void cancel() noexcept override {}
	};
}

TEST_SUITE("SendQueue")
{
	using namespace kmMqtt::mqtt;

	TEST_CASE("Socket send failure with positive OS error code is not treated as bytes sent")
	{
		auto socket = std::make_shared<MockSendSocket>();
		SendQueue queue;
		queue.setSocket(socket);
		queue.addToQueue(std::make_unique<MockPacketComposer>());

		SendBatchResult result;
		queue.sendNextBatch(result);

		CHECK(result.controlPacketSent == false);
		CHECK(result.totalBytesSent == 0U);
		CHECK(result.socketError == socket->lastErrorValue);
		CHECK(result.lastSendResult.noSendReason == NoSendReason::SOCKET_SEND_ERROR);
		CHECK(result.lastSendResult.socketError == socket->lastErrorValue);
		CHECK(socket->sendCalls > 0);
	}
}
