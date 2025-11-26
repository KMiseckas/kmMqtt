#include <cleanMqtt/Mqtt/Packets/Publish/Headers/PubRecVariableHeader.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		PubRecVariableHeader::PubRecVariableHeader() noexcept
		{
		}

		PubRecVariableHeader::PubRecVariableHeader(
			std::uint16_t packetId,
			PubRecReasonCode reasonCode,
			Properties&& properties) noexcept
			: packetId{ packetId },
			reasonCode{ reasonCode },
			properties{ std::move(properties) }
		{
		}

		DecodeResult PubRecVariableHeader::decode(const ByteBuffer& buffer) noexcept
		{
			DecodeResult result;

			packetId = buffer.readUInt16();

			if (packetId == 0)
			{
				result.code = DecodeErrorCode::PROTOCOL_ERROR;
				result.reason = "Packet ID cannot be zero in PUBREC";
				return result;
			}

			if (buffer.readHeadroom() > 0)
			{
				reasonCode = static_cast<PubRecReasonCode>(buffer.readUint8());
				result = properties.decode(buffer);
			}
			else
			{
				//If there are no more bytes, the reason code is SUCCESS by default (and property length can be ommitted per mqtt 5)
				reasonCode = PubRecReasonCode::SUCCESS;
			}

			return result;
		}

		void PubRecVariableHeader::encode(ByteBuffer& buffer) const
		{
			buffer.append(packetId);

			if (reasonCode == PubRecReasonCode::SUCCESS && properties.size() == 0)
			{
				return;
			}

			buffer += static_cast<std::uint8_t>(reasonCode);
			properties.encode(buffer);
		}

		std::size_t PubRecVariableHeader::getEncodedBytesSize() const noexcept
		{
			if (reasonCode == PubRecReasonCode::SUCCESS && properties.size() == 0)
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
