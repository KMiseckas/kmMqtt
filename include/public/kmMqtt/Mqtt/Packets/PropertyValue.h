// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_PACKETS_PROPERTYVALUE_H
#define INCLUDE_KMMQTT_MQTT_PACKETS_PROPERTYVALUE_H

#include <kmMqtt/Mqtt/Packets/PropertyType.h>

namespace kmMqtt
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

#endif //INCLUDE_KMMQTT_MQTT_PACKETS_PROPERTYVALUE_H
