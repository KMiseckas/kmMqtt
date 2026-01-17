#include <kmMqtt/Mqtt/Packets/Publish/Headers/PubAckVariableHeader.h>

namespace kmMqtt
{
	namespace mqtt
	{
		PubAckVariableHeader::PubAckVariableHeader() noexcept
		{
		}

		PubAckVariableHeader::PubAckVariableHeader(
			std::uint16_t packetId,
			PubAckReasonCode reasonCode,
			Properties&& properties) noexcept
			: packetId{ packetId },
			reasonCode{ reasonCode },
			properties{ std::move(properties) }
		{
		}

		DecodeResult PubAckVariableHeader::decode(const ByteBuffer& buffer) noexcept
		{
			DecodeResult result;

			packetId = buffer.readUInt16();

			if (packetId == 0)
			{
				result.code = DecodeErrorCode::PROTOCOL_ERROR;
				result.reason = "Packet ID cannot be zero in PUBACK";
				return result;
			}

			if (buffer.readHeadroom() > 0)
			{
				reasonCode = static_cast<PubAckReasonCode>(buffer.readUint8());
				result = properties.decode(buffer);
			}
			else
			{
				//If there are no more bytes, the reason code is SUCCESS by default (and property length can be ommitted per mqtt 5)
				reasonCode = PubAckReasonCode::SUCCESS;
			}

			return result;
		}

		void PubAckVariableHeader::encode(ByteBuffer& buffer) const
		{
			buffer.append(packetId);

			if (reasonCode == PubAckReasonCode::SUCCESS && properties.size() == 0)
			{
				return;
			}

			buffer += static_cast<std::uint8_t>(reasonCode);
			properties.encode(buffer);
		}

		std::size_t PubAckVariableHeader::getEncodedBytesSize() const noexcept
		{
			if(reasonCode == PubAckReasonCode::SUCCESS && properties.size() == 0)
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
