#include <cleanMqtt/Mqtt/Packets/PropertyType.h>
#include <cassert>

namespace cleanMqtt
{
	namespace mqtt
	{
		void propertyEncodings::encode(ByteBuffer& buffer, const PropertyType& property, const void* data)
		{
			k_propertyTypeEncodersZeroIndexed[k_propertyTypeZeroedId[static_cast<std::uint8_t>(property)]](buffer, property, data);
		}

		void* propertyDecodings::decode(const ByteBuffer& buffer, const PropertyType& property)
		{
			return k_propertyTypeDecodersZeroIndexed[k_propertyTypeZeroedId[static_cast<std::uint8_t>(property)]](buffer);
		}

		void propertyDestructors::destruct(void* data, const PropertyType& property)
		{
			return k_propertyTypeDestructorsZeroIndexed[k_propertyTypeZeroedId[static_cast<std::uint8_t>(property)]](data);
		}
	}
}
