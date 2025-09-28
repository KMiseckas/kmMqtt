#include <cleanMqtt/Mqtt/Transport/Jobs/SendPubAckJob.h>
#include <cleanMqtt/Mqtt/PacketHelper.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		SendResultData SendPubAckJob::send() noexcept
		{
			packets::PublishAck packet{ createPubAckPacket(m_publishPacketId, m_reasonCode, m_options) };
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

			int sendResult{ m_packetSendCallback(packet) };

			return SendResultData{
				packetSize,
				sendResult == 0,
				sendResult == 0 ? NoSendReason::NONE : NoSendReason::SOCKET_SEND_ERROR,
				std::move(result),
				sendResult };
		}

		void SendPubAckJob::cancel() noexcept
		{
		}
	}
}
