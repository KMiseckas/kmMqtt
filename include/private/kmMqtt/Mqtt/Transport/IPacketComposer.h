// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_INTERFACES_ISENDJOB_H
#define INCLUDE_KMMQTT_INTERFACES_ISENDJOB_H

#include <kmMqtt/GlobalMacros.h>
#include <kmMqtt/Mqtt/Packets/BasePacket.h>
#include <kmMqtt/Mqtt/MqttConnectionInfo.h>
#include <cstdint>
#include <functional>
#include <kmMqtt/Mqtt/Packets/ErrorCodes.h>
#include <kmMqtt/Mqtt/Transport/SendResultData.h>

namespace kmMqtt
{
	namespace mqtt
	{
		/**
		 * @brief Result of a packet composition job.
		 * Used internally in SendQueue to check the outcome of the packet composition.
		 */
		struct ComposeResult
		{
			DELETE_COPY_ASSIGNMENT_AND_CONSTRUCTOR(ComposeResult)

			ComposeResult(const EncodeResult& result, ByteBuffer data) noexcept
				: encodeResult{ result }
			{
				encodedData = std::move(data);
			}

			ComposeResult(ComposeResult&& other) noexcept
				: encodeResult{ other.encodeResult }, encodedData{ std::move(other.encodedData) }
			{
			}

			ComposeResult& operator=(ComposeResult&& other) noexcept
			{
				if (this != &other)
				{
					encodeResult = other.encodeResult;
					encodedData = std::move(other.encodedData);
				}
				return *this;
			}

			ComposeResult() = delete;

			EncodeResult encodeResult;
			ByteBuffer encodedData;
		};

		/**
		 * @brief Interface for packet composer jobs.
		 * Used internally in SendQueue to create packets to be sent over the network.
		 */
		class IPacketComposer
		{
		public:
			DELETE_MOVE_ASSIGNMENT_AND_CONSTRUCTOR(IPacketComposer)
			DELETE_COPY_ASSIGNMENT_AND_CONSTRUCTOR(IPacketComposer)

			IPacketComposer(MqttConnectionInfo* connectionInfo) noexcept
				: m_mqttConnectionInfo{ std::move(connectionInfo) }
			{
			};
			virtual ~IPacketComposer() {};

			/**
			 * @brief Can this packet be sent, or should it be skipped for now.
			 * Does not remove the job from the send queue, just skips it for now.
			 * 
			 * @return true if the packet can be sent, false otherwise.
			 */
			virtual bool canSend() const noexcept {return true;};

			/**
			 * @brief Get the QOS of the packet if it has one. Default to QOS_0.
			 * 
			 * @return Qos. Defaults to Qos::QOS_0 if packet doesnt have QOS feature.
			 */
			virtual Qos getQos() const noexcept { return Qos::QOS_0; };

			/**
			 * @brief Compose the packet ready for sending across the network.
			 * 
			 * @return The result of the composition job.
			 */
			virtual ComposeResult compose() noexcept = 0;

			/**
			 * @brief Cancel job, run any exit logic.
			 */
			virtual void cancel() noexcept = 0;

		protected:
			MqttConnectionInfo* m_mqttConnectionInfo;
		};
	}
}

#endif //INCLUDE_KMMQTT_INTERFACES_ISENDJOB_H
