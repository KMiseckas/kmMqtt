#include <cleanMqtt/Mqtt/Transport/Jobs/SendSubscribeJob.h>
#include <cleanMqtt/Mqtt/PacketHelper.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		SendSubscribeJob::SendSubscribeJob(
			MqttConnectionInfo* connectionInfo,
			PacketSendDelegate sendPacketCallback,
			PacketIdPool* packetIdPool,
			const PacketID packetId,
			std::vector<Topic> topics,
			SubscribeOptions&& subscribeOptions) noexcept :
			interfaces::ISendJob(connectionInfo, sendPacketCallback),
			m_packetIdPool{ packetIdPool },
			m_packetId{ packetId },
			m_topics{ std::move(topics) },
			m_subscribeOptions{ std::move(subscribeOptions) }
		{
		}

		interfaces::SendResultData SendSubscribeJob::send() noexcept
		{
			packets::Subscribe packet{ createSubscribePacket(m_packetId, m_topics, m_subscribeOptions) };
			EncodeResult result{ packet.encode() };

			if (!result.isSuccess())
			{
				return interfaces::SendResultData{
					0,
					false,
					interfaces::NoSendReason::ENCODE_ERROR,
					std::move(result),
					0
				};
			}

			const std::size_t packetSize{ PACKET_SIZE_POST_ENCODE(packet) };
			CHECK_ENFORCE_MAX_PACKET_SIZE(result, packetSize, m_mqttConnectionInfo->maxServerPacketSize);

			const int sendResult{ m_packetSendCallback(packet) };

			return interfaces::SendResultData{
				packetSize,
				sendResult == 0,
				sendResult == 0 ? interfaces::NoSendReason::NONE : interfaces::NoSendReason::SOCKET_SEND_ERROR,
				std::move(result),
				sendResult
			};
		}

		void SendSubscribeJob::cancel() noexcept
		{
			m_packetIdPool->releaseId(m_packetId);
		}
	}
}
