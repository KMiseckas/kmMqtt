#include <cleanMqtt/Mqtt/Packets/PropertyType.h>
#include <cassert>

namespace cleanMqtt
{
	namespace mqtt
	{
		void propertyEncodings::encode(ByteBuffer& buffer, PropertyType property, const void* data)
		{
			k_propertyTypeEncodersZeroIndexed[k_propertyTypeZeroedId[static_cast<std::uint8_t>(property)]](buffer, property, data);
		}

		void* propertyDecodings::decode(const ByteBuffer& buffer, PropertyType property)
		{
			return k_propertyTypeDecodersZeroIndexed[k_propertyTypeZeroedId[static_cast<std::uint8_t>(property)]](buffer);
		}
	}
}
