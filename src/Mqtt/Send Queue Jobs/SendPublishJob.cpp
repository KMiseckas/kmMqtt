#include <cleanMqtt/Mqtt/Send Queue Jobs/SendPublishJob.h>
#include <cleanMqtt/Mqtt/PacketHelper.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		SendPublishJob::SendPublishJob(MqttConnectionInfo* connectionInfo,
			PacketSendDelegate sendPacketCallback,
			PacketIdPool* idPool,
			const char* topic,
			ByteBuffer&& payload,
			PublishOptions&& pubOptions,
			bool enforceMaxPacketSize,
			std::size_t maxPacketSize) noexcept :
			interfaces::ISendJob(connectionInfo, sendPacketCallback, enforceMaxPacketSize, maxPacketSize),
			m_packetIdPool{ idPool },
			m_topic{ topic },
			m_payload{ std::move(payload) },
			m_publishOptions{ std::move(pubOptions) }
		{
		}

		interfaces::SendResultData SendPublishJob::send() noexcept
		{
			PacketID packetId{ 0U };
			if (m_publishOptions.qos >= mqtt::Qos::QOS_1)
			{
				packetId = m_packetIdPool->getId();
			}

			packets::Publish packet{ createPublishPacket(*m_mqttConnectionInfo, m_topic, std::move(m_payload), m_publishOptions, packetId)};
			EncodeResult result{ packet.encode() };

			if (!result.isSuccess())
			{
				m_packetIdPool->releaseId(packet.getVariableHeader().packetIdentifier);

				return interfaces::SendResultData{
				0,
				false,
				interfaces::NoSendReason::INTERNAL_ERROR,
				0 };
			}

			const std::size_t packetSize{ PACKET_SIZE_POST_ENCODE(packet) };

			CHECK_ENFORCE_MAX_PACKET_SIZE(m_enforcePacketSize, packetSize, m_maxPacketSize);

			int sendResult{ m_packetSendCallback(packet) };
			if (sendResult != 0)
			{
				m_packetIdPool->releaseId(packet.getVariableHeader().packetIdentifier);
			}

			return interfaces::SendResultData{
				packetSize,
				sendResult == 0,
				sendResult == 0 ? interfaces::NoSendReason::NONE : interfaces::NoSendReason::SOCKET_SEND_ERROR,
				sendResult };
		}
	}
}
