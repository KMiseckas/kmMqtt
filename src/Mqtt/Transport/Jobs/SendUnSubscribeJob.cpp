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
			const std::uint16_t packetId,
			std::vector<Topic> topics,
			UnSubscribeOptions&& options) noexcept :
			ISendJob(connectionInfo, sendPacketCallback),
			m_packetIdPool{ packetIdPool },
			m_packetId{ packetId },
			m_topics{ std::move(topics) },
			m_unSubscribeOptions{ std::move(options) }
		{
		}

		SendResultData SendUnSubscribeJob::send() noexcept
		{
			UnSubscribe packet{ createUnSubscribePacket(m_packetId, m_topics, m_unSubscribeOptions) };
			EncodeResult result{ packet.encode() };

			if (!result.isSuccess())
			{
				return SendResultData{
					0,
					false,
					NoSendReason::ENCODE_ERROR,
					std::move(result),
					0
				};
			}

			const std::size_t packetSize{ PACKET_SIZE_POST_ENCODE(packet) };
			CHECK_ENFORCE_MAX_PACKET_SIZE(result, packetSize, m_mqttConnectionInfo->maxServerPacketSize);

			const int sendResult{ m_packetSendCallback(packet) };

			return SendResultData{
				packetSize,
				sendResult == 0,
				sendResult == 0 ? NoSendReason::NONE : NoSendReason::SOCKET_SEND_ERROR,
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
