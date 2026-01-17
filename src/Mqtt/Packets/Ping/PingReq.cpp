#include <kmMqtt/Mqtt/Packets/Ping/PingReq.h>

namespace kmMqtt
{
	namespace mqtt
	{
		PingReq::PingReq() noexcept
			: BasePacket(FixedHeaderFlags(k_PingReqFixedHeaderFlags))
		{
		}

		PingReq::PingReq(ByteBuffer&& dataBuffer) noexcept
			: BasePacket(std::move(dataBuffer))
		{
		}

		PingReq::PingReq(PingReq&& other) noexcept
			: BasePacket{ std::move(other) }
		{
		}

		PingReq::~PingReq()
		{
		}

		PacketType PingReq::getPacketType() const noexcept
		{
			return PacketType::PING_REQUQEST;
		}
	}
}