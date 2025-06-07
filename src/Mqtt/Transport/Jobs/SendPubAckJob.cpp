#include <cleanMqtt/Mqtt/Transport/Jobs/SendPubAckJob.h>
#include <cleanMqtt/Mqtt/PacketHelper.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		interfaces::SendResultData SendPubAckJob::send() noexcept
		{
			//packets::PingReq packet{ createPubAckPacket(m_publishPacketId) };
			//EncodeResult result{ packet.encode() };

			//if (!result.isSuccess())
			//{
			//	return interfaces::SendResultData{
			//	0,
			//	false,
			//	interfaces::NoSendReason::ENCODE_ERROR,
			//	std::move(result),
			//	0 };
			//}

			//const std::size_t packetSize{ PACKET_SIZE_POST_ENCODE(packet) };

			//int sendResult{ m_packetSendCallback(packet) };

			/*return interfaces::SendResultData{
				packetSize,
				sendResult == 0,
				sendResult == 0 ? interfaces::NoSendReason::NONE : interfaces::NoSendReason::SOCKET_SEND_ERROR,
				std::move(result),
				sendResult };*/

			return interfaces::SendResultData{
				0,
				0 == 0,
				0 == 0 ? interfaces::NoSendReason::NONE : interfaces::NoSendReason::SOCKET_SEND_ERROR,
				{},
				0 };
		}

		void SendPubAckJob::cancel() noexcept
		{
		}
	}
}
