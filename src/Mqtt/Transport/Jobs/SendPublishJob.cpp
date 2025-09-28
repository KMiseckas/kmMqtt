#include <cleanMqtt/Mqtt/Transport/Jobs/SendPublishJob.h>
#include <cleanMqtt/Mqtt/PacketHelper.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		SendPublishJob::SendPublishJob(MqttConnectionInfo* connectionInfo,
			PacketSendDelegate sendPacketCallback,
			PacketIdPool* packetIdPool,
			const std::uint16_t packetId,
			const char* topic,
			ByteBuffer&& payload,
			PublishOptions&& pubOptions) noexcept :
			ISendJob(connectionInfo, sendPacketCallback),
			m_packetIdPool{ packetIdPool },
			m_packetId{ packetId },
			m_topic{ topic },
			m_payload{ std::move(payload) },
			m_publishOptions{ std::move(pubOptions) }
		{
		}

		SendResultData SendPublishJob::send() noexcept
		{
			packets::Publish packet{ createPublishPacket(*m_mqttConnectionInfo, m_topic, m_payload, m_publishOptions, m_packetId)};
			EncodeResult result{ packet.encode() };

			if (!result.isSuccess())
			{
				return SendResultData{
				0,
				false,
				NoSendReason::ENCODE_ERROR,
				std::move(result),
				0 };
			}

			const std::size_t packetSize{ PACKET_SIZE_POST_ENCODE(packet) };
			CHECK_ENFORCE_MAX_PACKET_SIZE(result, packetSize, m_mqttConnectionInfo->maxServerPacketSize);

			int sendResult{ m_packetSendCallback(packet) }; //Send the packet

			return SendResultData{
				packetSize,
				sendResult == 0,
				sendResult == 0 ? NoSendReason::NONE : NoSendReason::SOCKET_SEND_ERROR,
				std::move(result),
				sendResult };
		}

		void SendPublishJob::cancel() noexcept
		{
			
		}
	}
}
