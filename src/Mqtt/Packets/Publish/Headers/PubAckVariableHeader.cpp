#include <cleanMqtt/Mqtt/Packets/Publish/Headers/PubAckVariableHeader.h>

namespace cleanMqtt
{
    namespace mqtt
    {
        namespace packets
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

                if(packetId == 0)
                {
                    result.code = DecodeErrorCode::PROTOCOL_ERROR;
					result.reason = "Packet ID cannot be zero in PUBACK";
                    return result;
				}

                if (buffer.readHeadroom() > 0)
                {
                    reasonCode = static_cast<PubAckReasonCode>(buffer.readUint8());
                }

                if (buffer.readHeadroom() > 0)
                {
                    result = properties.decode(buffer);
                }

                return result;
            }

            void PubAckVariableHeader::encode(ByteBuffer& buffer) const
            {
                buffer.append(packetId);
                
                if (reasonCode != PubAckReasonCode::SUCCESS && properties.size() == 0)
                {
					return;
				}

				buffer += static_cast<std::uint8_t>(reasonCode);
				properties.encode(buffer);
			}

            std::size_t PubAckVariableHeader::getEncodedBytesSize() const noexcept
            {
                static constexpr std::size_t size{ sizeof(packetId) + sizeof(reasonCode) };
				std::size_t encodedSize{ size };
                if (properties.size() > 0)
                {
                    encodedSize += properties.encodingSize();
                }
                return encodedSize;
            }
        }
    }
}
