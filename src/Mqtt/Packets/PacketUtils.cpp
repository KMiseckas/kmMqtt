#include <cleanMqtt/Mqtt/Packets/PacketUtils.h>
#include <cleanMqtt/Interfaces/ILogger.h>
#include <stdexcept>

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
		{
			cleanMqtt::mqtt::packets::PacketType checkPacketType(const std::uint8_t* data, std::size_t size)
			{
				if (size == 0)
				{
					Exception(LogLevel::Error, "Packet Type Check", std::invalid_argument("Data buffer size is 0. Cannot determine packet type from provided data buffer."));
				}

				return static_cast<PacketType>(data[0] >> 4);
			}
		}
	}
}