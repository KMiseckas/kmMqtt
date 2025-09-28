#ifndef CLEANMQTT_MQTT_TRANSPORT_SENDRESULTDATA_H
#define CLEANMQTT_MQTT_TRANSPORT_SENDRESULTDATA_H

namespace cleanMqtt
{
	namespace mqtt
	{
		enum class NoSendReason : std::uint8_t
		{
			NONE,
			SOCKET_SEND_ERROR,
			OVER_MAX_PACKET_SIZE,
			ENCODE_ERROR
		};

		struct PUBLIC_API SendResultData
		{
			SendResultData() noexcept = default;
			SendResultData(std::size_t size, bool sent, NoSendReason reason, mqtt::packets::EncodeResult encodeResult, int error = 0) noexcept
				: packetSize{ size }, wasSent{ sent }, noSendReason{ reason }, encodeResult{ encodeResult }, socketError{ error }
			{
			}

			std::size_t packetSize{ 0U };
			bool wasSent{ false };
			NoSendReason noSendReason{ NoSendReason::NONE };
			mqtt::packets::EncodeResult encodeResult;
			int socketError{ 0 };
		};
	}
}

#endif // CLEANMQTT_MQTT_TRANSPORT_SENDRESULTDATA_H
