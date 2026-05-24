// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <doctest.h>
#include <kmMqtt/Mqtt/Transport/SendQueue.h>
#include <algorithm>
#include <vector>

namespace
{
	using namespace kmMqtt;
	using namespace kmMqtt::mqtt;

	class MockSendSocket : public IWebSocket
	{
	public:
		bool connect(const Address&) noexcept override { return true; }
		int send(const ByteBuffer& data) noexcept override
		{
			++sendCalls;

			if (sendScriptIndex < sendScript.size())
			{
				const int scriptedSendResult = sendScript[sendScriptIndex++];
				if (scriptedSendResult < 0)
				{
					return scriptedSendResult;
				}

				const int maxSendBytes = static_cast<int>(data.size());
				return std::min(scriptedSendResult, maxSendBytes);
			}

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
		std::vector<int> sendScript;
		std::size_t sendScriptIndex{ 0U };
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

	class ConfigurablePacketComposer : public IPacketComposer
	{
	public:
		ConfigurablePacketComposer(const PacketType packetType, const std::uint16_t packetId, const std::size_t payloadSize) noexcept
			: IPacketComposer(nullptr), m_packetType(packetType), m_packetId(packetId), m_payloadSize(payloadSize)
		{
		}

		ComposeResult compose() noexcept override
		{
			EncodeResult result{ EncodeErrorCode::NO_ERROR };
			result.packetType = m_packetType;
			result.packetId = m_packetId;

			ByteBuffer data{ m_payloadSize };
			for (std::size_t i = 0; i < m_payloadSize; ++i)
			{
				data += static_cast<std::uint8_t>(0x00);
			}

			return ComposeResult{ result, std::move(data) };
		}

		void cancel() noexcept override {}

	private:
		PacketType m_packetType{ PacketType::RESERVED };
		std::uint16_t m_packetId{ 0U };
		std::size_t m_payloadSize{ 0U };
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
		CHECK(result.socketError == NO_SOCKET_ERROR);
		CHECK(result.lastSendResult.noSendReason == NoSendReason::SOCKET_SEND_ERROR);
		CHECK(result.lastSendResult.socketError == socket->lastErrorValue);
		CHECK(socket->sendCalls > 0);
	}

	TEST_CASE("Partial send metadata cleanup keeps callbacks consistent")
	{
		auto socket = std::make_shared<MockSendSocket>();
		socket->sendScript = { 7, 7, 7, 7, 7, 3 };

		SendQueue queue;
		queue.setSocket(socket);

		int disconnectSentCount{ 0 };
		int pubRecSentCount{ 0 };
		std::uint16_t lastPubRecPacketId{ 0U };

		queue.setOnPingSentCallback([]() {});
		queue.setOnPubCompSentCallback([](std::uint16_t) {});
		queue.setOnPubRelSentCallback([](std::uint16_t) {});
		queue.setOnPubRecSentCallback([&](const std::uint16_t packetId)
			{
				++pubRecSentCount;
				lastPubRecPacketId = packetId;
			});
		queue.setOnDisconnectSentCallback([&]() { ++disconnectSentCount; });

		queue.addToQueue(std::make_unique<ConfigurablePacketComposer>(PacketType::PING_REQUQEST, 0U, 2U));
		for (int i = 0; i < 10; ++i)
		{
			queue.addToQueue(std::make_unique<ConfigurablePacketComposer>(PacketType::CONNECT, 0U, 2U));
		}
		queue.addToQueue(std::make_unique<ConfigurablePacketComposer>(PacketType::DISCONNECT, 0U, 2U));

		SendBatchResult firstBatchResult;
		queue.sendNextBatch(firstBatchResult);
		CHECK(firstBatchResult.controlPacketSent == true);

		queue.addToQueue(std::make_unique<ConfigurablePacketComposer>(PacketType::PING_REQUQEST, 0U, 2U));
		queue.addToQueue(std::make_unique<ConfigurablePacketComposer>(PacketType::PUBLISH_RECEIVED, 77U, 2U));
		queue.addToQueue(std::make_unique<ConfigurablePacketComposer>(PacketType::CONNECT, 0U, 4U));

		SendBatchResult secondBatchResult;
		queue.sendNextBatch(secondBatchResult);

		CHECK(secondBatchResult.controlPacketSent == true);
		CHECK(pubRecSentCount == 1);
		CHECK(lastPubRecPacketId == 77U);
		CHECK(disconnectSentCount == 1);
		CHECK(socket->sendCalls == 6);
	}
}
