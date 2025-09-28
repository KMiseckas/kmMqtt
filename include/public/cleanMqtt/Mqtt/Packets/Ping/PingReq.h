#ifndef INCLUDE_CLEANMQTT_MQTT_PACKETS_PING_PINGREQ_H
#define INCLUDE_CLEANMQTT_MQTT_PACKETS_PING_PINGREQ_H

#include <cleanMqtt/Mqtt/Packets/BasePacket.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
		{
			namespace
			{
				constexpr std::uint8_t k_PingReqFixedHeaderFlags = 0U;
			}

			class PingReq : public BasePacket
			{
			public:
				PingReq() noexcept;
				PingReq(ByteBuffer&& dataBuffer) noexcept;
				PingReq(PingReq&& other) noexcept;
				~PingReq() override;

				PacketType getPacketType() const noexcept override;
			};
		}
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETS_PING_PINGREQ_H