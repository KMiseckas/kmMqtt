// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <kmMqtt/Mqtt/Packets/Publish/Headers/PubRelVariableHeader.h>

namespace kmMqtt
{
	namespace mqtt
	{
		PubRelVariableHeader::PubRelVariableHeader() noexcept
		{
		}

		PubRelVariableHeader::PubRelVariableHeader(
			std::uint16_t packetId,
			PubRelReasonCode reasonCode,
			Properties&& properties) noexcept
			: packetId{ packetId },
			reasonCode{ reasonCode },
			properties{ std::move(properties) }
		{
		}

		DecodeResult PubRelVariableHeader::decode(const ByteBuffer& buffer) noexcept
		{
			DecodeResult result;

			packetId = buffer.readUInt16();

			if (packetId == 0)
			{
				result.code = DecodeErrorCode::PROTOCOL_ERROR;
				result.reason = "Packet ID cannot be zero in PUBREL";
				return result;
			}

			if (buffer.readHeadroom() > 0)
			{
				reasonCode = static_cast<PubRelReasonCode>(buffer.readUint8());
				result = properties.decode(buffer);
			}
			else
			{
				//If there are no more bytes, the reason code is SUCCESS by default (and property length can be ommitted per mqtt 5)
				reasonCode = PubRelReasonCode::SUCCESS;
			}

			return result;
		}

		void PubRelVariableHeader::encode(ByteBuffer& buffer) const
		{
			buffer.append(packetId);

			if (reasonCode == PubRelReasonCode::SUCCESS && properties.size() == 0)
			{
				return;
			}

			buffer += static_cast<std::uint8_t>(reasonCode);
			properties.encode(buffer);
		}

		std::size_t PubRelVariableHeader::getEncodedBytesSize() const noexcept
		{
			if (reasonCode == PubRelReasonCode::SUCCESS && properties.size() == 0)
			{
				return sizeof(packetId);
			}

			static constexpr std::size_t size{ sizeof(packetId) + sizeof(reasonCode) };
			std::size_t encodedSize{ size };
			encodedSize += properties.encodingSize();
			return encodedSize;
		}
	}
}
