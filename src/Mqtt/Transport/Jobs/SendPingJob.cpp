#include <cleanMqtt/Mqtt/Transport/Jobs/SendPingJob.h>
#include <cleanMqtt/Mqtt/PacketHelper.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		interfaces::SendResultData SendPingJob::send() noexcept
		{
			packets::PingReq packet{ createPingRequestPacket() };
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

			int sendResult{ m_packetSendCallback(packet) };

			return interfaces::SendResultData{
				packetSize,
				sendResult == 0,
				sendResult == 0 ? interfaces::NoSendReason::NONE : interfaces::NoSendReason::SOCKET_SEND_ERROR,
				std::move(result),
				sendResult };
		}

		void SendPingJob::cancel() noexcept
		{
			m_mqttConnectionInfo->lastPingReqSentTime = std::chrono::steady_clock::now();
			m_mqttConnectionInfo->awaitingPingResponse = true;
		}
	}
}
