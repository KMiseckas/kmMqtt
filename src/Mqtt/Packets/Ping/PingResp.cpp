#include <kmMqtt/Mqtt/Packets/Ping/PingResp.h>

namespace kmMqtt
{
	namespace mqtt
	{
		PingResp::PingResp() noexcept
			: BasePacket(FixedHeaderFlags(k_PingRespFixedHeaderFlags))
		{
		}

		PingResp::PingResp(ByteBuffer&& dataBuffer) noexcept
			: BasePacket(std::move(dataBuffer))
		{
		}

		PingResp::PingResp(PingResp&& other) noexcept
			: BasePacket{ std::move(other) }
		{
		}

		PingResp::~PingResp()
		{
		}

		PacketType PingResp::getPacketType() const noexcept
		{
			return PacketType::PING_RESPONSE;
		}
	}
}