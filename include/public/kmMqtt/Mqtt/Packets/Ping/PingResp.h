#ifndef INCLUDE_KMMQTT_MQTT_PACKETS_PING_PINGRESP_H
#define INCLUDE_KMMQTT_MQTT_PACKETS_PING_PINGRESP_H

#include <kmMqtt/Mqtt/Packets/BasePacket.h>

namespace kmMqtt
{
	namespace mqtt
	{
		namespace
		{
			constexpr std::uint8_t k_PingRespFixedHeaderFlags = 0U;
		}

		class PingResp : public BasePacket
		{
		public:
			PingResp() noexcept;
			PingResp(ByteBuffer&& dataBuffer) noexcept;
			PingResp(PingResp&& other) noexcept;
			~PingResp() override;

			PacketType getPacketType() const noexcept override;
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_PACKETS_PING_PINGRESP_H