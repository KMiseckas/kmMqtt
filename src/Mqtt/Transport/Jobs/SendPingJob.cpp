#include <cleanMqtt/Mqtt/Transport/Jobs/SendPingJob.h>
#include <cleanMqtt/Mqtt/PacketHelper.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		SendResultData SendPingJob::send() noexcept
		{
			packets::PingReq packet{ createPingRequestPacket() };
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

			int sendResult{ m_packetSendCallback(packet) };

			if (sendResult == 0)
			{
				m_mqttConnectionInfo->lastPingReqSentTime = std::chrono::steady_clock::now();
				m_mqttConnectionInfo->awaitingPingResponse = true;
			}

			return SendResultData{
				packetSize,
				sendResult == 0,
				sendResult == 0 ? NoSendReason::NONE : NoSendReason::SOCKET_SEND_ERROR,
				std::move(result),
				sendResult };
		}

		void SendPingJob::cancel() noexcept
		{
		}
	}
}
