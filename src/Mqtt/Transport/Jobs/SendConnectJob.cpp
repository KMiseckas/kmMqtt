#include <cleanMqtt/Mqtt/Transport/Jobs/SendConnectJob.h>
#include <cleanMqtt/Mqtt/PacketHelper.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		interfaces::SendResultData SendConnectJob::send() noexcept
		{
			packets::Connect packet{ createConnectPacket(*m_mqttConnectionInfo) };
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
			CHECK_ENFORCE_MAX_PACKET_SIZE(result, packetSize, m_mqttConnectionInfo->maxServerPacketSize);

			int sendResult{ m_packetSendCallback(packet) };

			return interfaces::SendResultData{
				packetSize,
				sendResult == 0,
				sendResult == 0 ? interfaces::NoSendReason::NONE : interfaces::NoSendReason::SOCKET_SEND_ERROR,
				std::move(result),
				sendResult };
		}

		void SendConnectJob::cancel() noexcept
		{
			// No specific cancellation logic for Connect job
		}
	}
}
