#include <cleanMqtt/Mqtt/Transport/Jobs/SendUnSubscribeJob.h>
#include <cleanMqtt/Mqtt/PacketHelper.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		SendUnSubscribeJob::SendUnSubscribeJob(
			MqttConnectionInfo* connectionInfo,
			PacketSendDelegate sendPacketCallback,
			PacketIdPool* packetIdPool,
			const PacketID packetId,
			std::vector<Topic> topics,
			UnSubscribeOptions&& options) noexcept :
			interfaces::ISendJob(connectionInfo, sendPacketCallback),
			m_packetIdPool{ packetIdPool },
			m_packetId{ packetId },
			m_topics{ std::move(topics) },
			m_unSubscribeOptions{ std::move(options) }
		{
		}

		interfaces::SendResultData SendUnSubscribeJob::send() noexcept
		{
			packets::UnSubscribe packet{ createUnSubscribePacket(m_packetId, m_topics, m_unSubscribeOptions) };
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

		void SendUnSubscribeJob::cancel() noexcept
		{
			m_packetIdPool->releaseId(m_packetId);
		}
	}
}
