#include <cleanMqtt/Mqtt/Send Queue Jobs/SendConnectJob.h>
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
				interfaces::NoSendReason::INTERNAL_ERROR,
				0 };
			}

			const std::size_t packetSize{ PACKET_SIZE_POST_ENCODE(packet) };

			CHECK_ENFORCE_MAX_PACKET_SIZE(m_enforcePacketSize, packetSize, m_maxPacketSize);

			int sendResult{ m_packetSendCallback(packet) };

			return interfaces::SendResultData{
				packetSize,
				sendResult == 0,
				sendResult == 0 ? interfaces::NoSendReason::NONE : interfaces::NoSendReason::SOCKET_SEND_ERROR,
				sendResult };
		}
	}
}
