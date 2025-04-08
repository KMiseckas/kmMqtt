#include <cleanMqtt/Mqtt/Packets/Publish/Publish.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
		{
			Publish::Publish(const char* /*topic*/, const char* /*msg*/, const EncodedPublishFlags& flags) noexcept
				: BasePacket(flags)
			{
			}

			Publish::~Publish()
			{
			}

			PacketType Publish::getPacketType() const noexcept
			{
				return PacketType::PUBLISH;
			}
		}
	}
}