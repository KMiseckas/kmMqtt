#ifndef INCLUDE_CLEANMQTT_MQTT_PACKETS_PROPERTYVALUE_H
#define INCLUDE_CLEANMQTT_MQTT_PACKETS_PROPERTYVALUE_H

#include <cleanMqtt/Mqtt/Packets/PropertyType.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		class PropertyValue
		{
		public:
			explicit PropertyValue(void* value, const PropertyType& type)
				: m_value(value), m_propertyType(type)
			{
			}

			~PropertyValue()
			{
				if (m_value)
				{
					propertyDestructors::destruct(m_value, m_propertyType);
				}
			}

			operator const void* () const { return m_value; }

		private:
			void* m_value;
			PropertyType m_propertyType;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETS_PROPERTYVALUE_H
