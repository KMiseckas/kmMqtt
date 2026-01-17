// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_SENDQUEUE_H
#define INCLUDE_KMMQTT_MQTT_SENDQUEUE_H

#include <kmMqtt/GlobalMacros.h>
#include <kmMqtt/Mqtt/Transport/IPacketComposer.h>
#include <kmMqtt/Interfaces/IWebSocket.h>
#include <cstdint>
#include <chrono>
#include <memory>
#include <string>

namespace kmMqtt
{
	namespace mqtt
	{
#define NO_SOCKET_ERROR 0U

		struct SendBatchResult
		{
			bool controlPacketSent{ false };
			std::size_t totalBytesSent{ 0U };
			int socketError{ NO_SOCKET_ERROR };
			bool isRecoverable{ true };
			std::string unrecoverableReasonStr;
			SendResultData lastSendResult;
		};

		using PacketSendJobPtr = std::unique_ptr<IPacketComposer>;

		struct ReceiveMaximumTracker;

		/**
		 * @brief Handles the processing and sending of queued up MQTT packets.
		 * - Stores a queue of packet composers, these are run to construct the encoded packet ready for transport across the network.
		 * - Encoded packets are first merged into a single byte buffer to minimise send calls.
		 * - Tries to send the whole buffer across, but if unsuccesfull will proccess the remaining bytes next tick.
		 * 
		 * //TODO possible overflow if sending doesnt catch up to data queued up?
		 */
		class SendQueue
		{
			struct PacketSectionMetadata
			{
				std::size_t endByteInBuffer{ 0U };
				PacketType packetType{ PacketType::RESERVED };
				std::uint16_t packetId{ 0U };
			};

		public:
			SendQueue() noexcept;
			virtual ~SendQueue();

			void setSocket(std::shared_ptr<IWebSocket> socket) noexcept;
			void setReceiveMaximumTracker(ReceiveMaximumTracker* const tracker) noexcept;
			void addToQueue(PacketSendJobPtr packetSendJob);
			void sendNextBatch(SendBatchResult& outResult);
			void clearQueue(const bool graceful = false) noexcept;

			void setOnPingSentCallback(const std::function<void()>& callback) noexcept;
			void setOnPubCompSentCallback(const std::function<void(std::uint16_t)>& callback) noexcept;
			void setOnPubRelSentCallback(const std::function<void(std::uint16_t)>& callback) noexcept;
			void setOnPubRecSentCallback(const std::function<void(std::uint16_t)>& callback) noexcept;
			void setOnDisconnectSentCallback(const std::function<void()>& callback) noexcept;

		private:
			bool trySendBatch(SendBatchResult& outResult, SendResultData& outLastSendResult);
			int sendData(const ByteBuffer& data);

			std::shared_ptr<IWebSocket> m_socket;
			std::function<void()> m_onPingSentCallback;
			std::function<void(std::uint16_t)> m_onPubCompSentCallback;
			std::function<void(std::uint16_t)> m_onPubRelSentCallback;
			std::function<void(std::uint16_t)> m_onPubRecSentCallback;
			std::function<void()> m_onDisconnectSentCallback;
			ByteBuffer m_sendBuffer;

			std::uint8_t m_currentLocalRetry{ 0U };
			SendResultData m_lastSendData{ 0, true, NoSendReason::NONE,{}, 0 };

			const std::uint8_t k_maxSendBatchRetries{ 3U };
			std::uint8_t m_sendBatchRetryCount{ 0U };
			const std::chrono::milliseconds k_retryDelayMs{ 250 };
			std::chrono::steady_clock::time_point m_lastRetryTime;

			std::vector<PacketSendJobPtr> m_nextPacketComposersBatch;
			std::vector<PacketSectionMetadata> m_packetsMetadataInBuffer;

			ReceiveMaximumTracker* m_receiveMaximumTrackerPtr{ nullptr };
			std::uint32_t m_receiveMaximumClient{ RECEIVE_MAXIMUM_DEFAULT };
			std::uint32_t m_receiveMaximumBroker{ RECEIVE_MAXIMUM_DEFAULT };

			std::mutex m_mutex;
			bool m_startGracefulClear{ false };
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_SENDQUEUE_H 
