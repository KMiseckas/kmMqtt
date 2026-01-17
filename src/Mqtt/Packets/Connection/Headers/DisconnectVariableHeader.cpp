// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <kmMqtt/Mqtt/Packets/Connection/Headers/DisconnectVariableHeader.h>

namespace kmMqtt
{
	namespace mqtt
	{
		DisconnectVariableHeader::DisconnectVariableHeader() noexcept
		{
		}

		DisconnectVariableHeader::DisconnectVariableHeader(DisconnectReasonCode disconnectReasonCode, Properties&& disconnectProperties) noexcept
			: reasonCode(disconnectReasonCode), properties(std::move(disconnectProperties))
		{
		}

		void DisconnectVariableHeader::encode(ByteBuffer& buffer) const
		{
			//Spec allows to skip encoding of data in following circumstances.
			if (reasonCode == DisconnectReasonCode::NORMAL_DISCONNECTION && properties.size() == 0)
			{
				return;
			}

			buffer += static_cast<std::uint8_t>(reasonCode);
			properties.encode(buffer);
		}

		std::size_t DisconnectVariableHeader::getEncodedBytesSize() const noexcept
		{
			//Spec allows to skip encoding of data in following circumstances.
			if (reasonCode == DisconnectReasonCode::NORMAL_DISCONNECTION && properties.size() == 0)
			{
				return 0;
			}

			return sizeof(reasonCode) +
				properties.encodingSize();
		}

		DecodeResult DisconnectVariableHeader::decode(const ByteBuffer& buffer) noexcept
		{
			DecodeResult result;

			reasonCode = static_cast<DisconnectReasonCode>(buffer.readUint8());
			result = properties.decode(buffer);

			return result;
		}
	}
}
