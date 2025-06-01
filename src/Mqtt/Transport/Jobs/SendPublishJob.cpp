#include <cleanMqtt/Mqtt/Transport/Jobs/SendPublishJob.h>
#include <cleanMqtt/Mqtt/PacketHelper.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		SendPublishJob::SendPublishJob(MqttConnectionInfo* connectionInfo,
			PacketSendDelegate sendPacketCallback,
			PacketIdPool* packetIdPool,
			const PacketID packetId,
			const char* topic,
			ByteBuffer&& payload,
			PublishOptions&& pubOptions,
			bool enforceMaxPacketSize,
			std::size_t maxPacketSize) noexcept :
			interfaces::ISendJob(connectionInfo, sendPacketCallback, enforceMaxPacketSize, maxPacketSize),
			m_packetIdPool{ packetIdPool },
			m_packetId{ packetId },
			m_topic{ topic },
			m_payload{ std::move(payload) },
			m_publishOptions{ std::move(pubOptions) }
		{
		}

		interfaces::SendResultData SendPublishJob::send() noexcept
		{
			packets::Publish packet{ createPublishPacket(*m_mqttConnectionInfo, m_topic, m_payload, m_publishOptions, m_packetId)};
			EncodeResult result{ packet.encode() };

			if (!result.isSuccess())
			{
				return interfaces::SendResultData{
				0,
				false,
				interfaces::NoSendReason::ENCODE_ERROR,
				std::move(result),
				0 };
			}

			const std::size_t packetSize{ PACKET_SIZE_POST_ENCODE(packet) };

			CHECK_ENFORCE_MAX_PACKET_SIZE(m_enforcePacketSize, result, packetSize, m_maxPacketSize);

			int sendResult{ m_packetSendCallback(packet) }; //Send the packet

			return interfaces::SendResultData{
				packetSize,
				sendResult == 0,
				sendResult == 0 ? interfaces::NoSendReason::NONE : interfaces::NoSendReason::SOCKET_SEND_ERROR,
				std::move(result),
				sendResult };
		}

		void SendPublishJob::cancel() noexcept
		{
			m_packetIdPool->releaseId(m_packetId);
		}
	}
}
