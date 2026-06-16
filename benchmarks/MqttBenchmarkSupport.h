// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#pragma once

#include <kmMqtt/ByteBuffer.h>
#include <kmMqtt/Config.h>
#include <kmMqtt/Interfaces/IAllocator.h>
#include <kmMqtt/Interfaces/IMqttEnvironment.h>
#include <kmMqtt/Interfaces/IWebSocket.h>
#include <kmMqtt/Memory/AllocatorContext.h>
#include <kmMqtt/Mqtt/Enums/ConnectionStatus.h>
#include <kmMqtt/Mqtt/Packets/PropertyType.h>
#include <kmMqtt/Mqtt/Params/ConnectAddress.h>
#include <kmMqtt/Mqtt/Params/ConnectArgs.h>
#include <kmMqtt/MqttClient.h>
#include <kmMqtt/MqttClientOptions.h>
#include <kmMqtt/STL/KmMemory.h>
#include <kmMqtt/STL/KmQueue.h>
#include <kmMqtt/STL/KmVector.h>

#include <cstddef>
#include <cstdint>
#include <new>
#include <utility>

namespace kmMqtt
{
	namespace benchmarkSupport
	{
		class BenchmarkWebSocket final : public IWebSocket
		{
		public:
			bool connect(const mqtt::Address& address) noexcept override
			{
				lastHost = address.hostname();
				lastPort = address.port();
				connected = true;

				if (onConnectCb)
				{
					onConnectCb(true);
				}

				return true;
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

				ByteBuffer copy{ data.size() };
				copy.append(data.bytes(), data.size());
				sentPackets.push_back(std::move(copy));

				return static_cast<int>(data.size());
			}

			bool close() noexcept override
			{
				connected = false;

				if (onDisconnectCb)
				{
					onDisconnectCb();
				}

				return true;
			}

			void tick() noexcept override
			{
				if (!connected || pendingResponses.empty())
				{
					return;
				}

				ByteBuffer response = std::move(pendingResponses.front());
				pendingResponses.pop();

				if (onPacketCb)
				{
					onPacketCb(std::move(response));
				}
			}

			bool isConnected() const noexcept override
			{
				return connected;
			}

			int getLastError() const noexcept override
			{
				return lastError;
			}

			int getLastCloseCode() const noexcept override
			{
				return lastCloseCode;
			}

			const char* getLastCloseReason() const noexcept override
			{
				return lastCloseReason.c_str();
			}

			void setOnConnectCallback(OnConnectCallback cb) noexcept override
			{
				onConnectCb = std::move(cb);
			}

			void setOnDisconnectCallback(OnDisconnectCallback cb) noexcept override
			{
				onDisconnectCb = std::move(cb);
			}

			void setOnRecvdCallback(OnRecvdCallback cb) noexcept override
			{
				onPacketCb = std::move(cb);
			}

			void setOnErrorCallback(OnErrorCallback cb) noexcept override
			{
				onErrorCb = std::move(cb);
			}

			void queueResponse(const ByteBuffer& data)
			{
				ByteBuffer copy{ data.size() };
				copy.append(data.bytes(), data.size());
				pendingResponses.push(std::move(copy));
			}

			kmStd::string lastHost;
			kmStd::string lastPort;
			kmStd::string lastCloseReason{ "benchmark" };
			kmStd::vector<ByteBuffer> sentPackets;
			kmStd::queue<ByteBuffer> pendingResponses;
			OnConnectCallback onConnectCb;
			OnDisconnectCallback onDisconnectCb;
			OnRecvdCallback onPacketCb;
			OnErrorCallback onErrorCb;
			bool connected{ false };
			int lastError{ 0 };
			int lastCloseCode{ 0 };
		};

		class BenchmarkEnvironment final : public IMqttEnvironment
		{
		public:
			Config createConfig() const noexcept override
			{
				return config;
			}

			kmStd::shared_ptr<IWebSocket> createWebSocket() const noexcept override
			{
				auto socket = kmStd::make_shared<BenchmarkWebSocket>();
				socketPtr = socket.get();
				return socket;
			}

			mutable BenchmarkWebSocket* socketPtr{ nullptr };
			Config config{};
		};

		template <std::size_t Capacity>
		class PreallocatedAllocator final : public IAllocator
		{
		public:
			void* allocate(std::size_t size, std::size_t alignment) override
			{
				if (alignment == 0U)
				{
					alignment = alignof(std::max_align_t);
				}

				const std::size_t requestedSize = size == 0U ? 1U : size;
				const std::size_t currentAddress = reinterpret_cast<std::size_t>(m_storage + m_offset);
				const std::size_t alignedAddress = (currentAddress + (alignment - 1U)) & ~(alignment - 1U);
				const std::size_t alignedOffset = alignedAddress - reinterpret_cast<std::size_t>(m_storage);

				if (alignedOffset + requestedSize > Capacity)
				{
					m_failed = true;
					return nullptr;
				}

				m_offset = alignedOffset + requestedSize;
				if (m_offset > m_peakOffset)
				{
					m_peakOffset = m_offset;
				}

				return m_storage + alignedOffset;
			}

			void deallocate(void*, std::size_t, std::size_t) noexcept override
			{
				// Monotonic arena: memory is reclaimed by reset() between benchmark iterations.
			}

			void reset() noexcept
			{
				m_offset = 0U;
				m_failed = false;
			}

			bool failed() const noexcept
			{
				return m_failed;
			}

			std::size_t peakBytesUsed() const noexcept
			{
				return m_peakOffset;
			}

			static constexpr std::size_t capacityBytes() noexcept
			{
				return Capacity;
			}

		private:
			alignas(std::max_align_t) unsigned char m_storage[Capacity]{};
			std::size_t m_offset{ 0U };
			std::size_t m_peakOffset{ 0U };
			bool m_failed{ false };
		};

		enum class ScenarioError : std::uint8_t
		{
			None,
			SocketUnavailable,
			ConnectFailed,
			ConnectTickFailed,
			HandshakeFailed,
			DisconnectFailed,
		};

		inline mqtt::ConnectArgs makeConnectArgs()
		{
			mqtt::ConnectArgs args{ "benchmark-client" };
			args.cleanStart = true;
			args.protocolName = "MQTT";
			args.version = mqtt::MqttVersion::MQTT_5_0;
			args.keepAliveInSec = 45U;
			args.receiveMaximum = 16U;
			args.maximumPacketSize = 4096U;
			args.maximumTopicAliases = 8U;
			args.requestProblemInformation = true;
			args.requestResponseInformation = false;
			args.username = "allocator-bench";
			args.password = "allocator-bench-password";
			args.userProperties.emplace("bench", "allocator");
			args.userProperties.emplace("mode", "sync");
			args.userProperties.emplace("scenario", "connect-disconnect");

			static const std::uint8_t willPayloadBytes[] = { 1U, 3U, 5U, 7U, 9U, 11U, 13U, 15U };
			args.will = kmStd::make_unique<mqtt::Will>("bench/will/topic");
			args.will->willQos = mqtt::Qos::QOS_1;
			args.will->retainWillMessage = true;
			args.will->contentType = "application/octet-stream";
			args.will->responseTopic = "bench/response/topic";
			args.will->payload = kmStd::make_unique<mqtt::BinaryData>(
				static_cast<std::uint16_t>(sizeof(willPayloadBytes)),
				willPayloadBytes);
			args.will->userProperties.emplace("will-key", "will-value");

			return args;
		}

		inline mqtt::ConnectAddress makeConnectAddress()
		{
			mqtt::ConnectAddress address{};
			address.primaryAddress = mqtt::Address::createURL("", "localhost", "1883", "");
			address.otherAddresses.push_back(mqtt::Address::createURL("", "localhost", "1884", ""));
			address.otherAddresses.push_back(mqtt::Address::createURL("", "localhost", "1885", ""));
			return address;
		}

		inline ByteBuffer makeConnectAck()
		{
			ByteBuffer buffer{ 16U };
			buffer += static_cast<std::uint8_t>(0x20U);
			buffer += static_cast<std::uint8_t>(14U);
			buffer += static_cast<std::uint8_t>(0U);
			buffer += static_cast<std::uint8_t>(0U);
			buffer += static_cast<std::uint8_t>(11U);
			buffer += static_cast<std::uint8_t>(mqtt::PropertyType::SESSION_EXPIRY_INTERVAL);
			buffer.append(static_cast<std::uint32_t>(30U));
			buffer += static_cast<std::uint8_t>(mqtt::PropertyType::RECEIVE_MAXIMUM);
			buffer.append(static_cast<std::uint16_t>(16U));
			buffer += static_cast<std::uint8_t>(mqtt::PropertyType::TOPIC_ALIAS_MAXIMUM);
			buffer.append(static_cast<std::uint16_t>(8U));
			return buffer;
		}

		inline ScenarioError runDeterministicConnectionScenario()
		{
			BenchmarkEnvironment environment{};
			MqttClientOptions clientOptions{ TickMode::SYNC };
			mqtt::MqttClient client{ &environment, clientOptions };

			if (environment.socketPtr == nullptr)
			{
				return ScenarioError::SocketUnavailable;
			}

			const auto connectResult = client.connect(makeConnectArgs(), makeConnectAddress());
			if (!connectResult.noError())
			{
				return ScenarioError::ConnectFailed;
			}

			environment.socketPtr->queueResponse(makeConnectAck());

			const auto firstTickResult = client.tick();
			if (!firstTickResult.noError())
			{
				return ScenarioError::ConnectTickFailed;
			}

			const auto secondTickResult = client.tick();
			if (!secondTickResult.noError())
			{
				return ScenarioError::ConnectTickFailed;
			}

			if (client.getConnectionStatus() != mqtt::ConnectionStatus::CONNECTED)
			{
				return ScenarioError::HandshakeFailed;
			}

			const auto disconnectResult = client.disconnect();
			if (!disconnectResult.noError())
			{
				return ScenarioError::DisconnectFailed;
			}

			return ScenarioError::None;
		}

		inline const char* toString(ScenarioError error) noexcept
		{
			switch (error)
			{
			case ScenarioError::None:
				return "none";
			case ScenarioError::SocketUnavailable:
				return "socket unavailable";
			case ScenarioError::ConnectFailed:
				return "connect failed";
			case ScenarioError::ConnectTickFailed:
				return "connect tick failed";
			case ScenarioError::HandshakeFailed:
				return "handshake failed";
			case ScenarioError::DisconnectFailed:
				return "disconnect failed";
			default:
				return "unknown";
			}
		}
	}
}
