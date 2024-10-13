#include <cleanMqtt/Mqtt/Packets/PropertyType.h>
#include <cassert>

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
		{
			void propertyEncodings::encode(ByteBuffer& buffer, PropertyType property, const void* data)
			{
				auto pair = k_propertyTraits.find(property);
				assert(pair != k_propertyTraits.end());

				pair->second.encoder(buffer, property, data);
			}

			void* propertyDecodings::decode(const ByteBuffer& buffer, PropertyType property)
			{
				auto pair = k_propertyTraits.find(property);
				assert(pair != k_propertyTraits.end());

				return pair->second.decoder(buffer);
			}
		}
	}
}
