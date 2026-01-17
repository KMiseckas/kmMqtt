#ifndef INCLUDE_KMMQTT_MQTT_PACKETS_PING_PINGREQ_H
#define INCLUDE_KMMQTT_MQTT_PACKETS_PING_PINGREQ_H

#include <kmMqtt/Mqtt/Packets/BasePacket.h>

namespace kmMqtt
{
	namespace mqtt
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

#endif //INCLUDE_KMMQTT_MQTT_PACKETS_PING_PINGREQ_H