#ifndef INCLUDE_CLEANMQTT_MQTT_PACKETS_PACKET_H
#define INCLUDE_CLEANMQTT_MQTT_PACKETS_PACKET_H

#include <cleanMqtt/ByteBuffer.h>
#include <cleanMqtt/Mqtt/Packets/PacketUtils.h>
#include <cleanMqtt/Mqtt/Packets/PacketType.h>
#include <cleanMqtt/Mqtt/Packets/FixedHeader.h>
#include <cleanMqtt/GlobalMacros.h>
#include <cleanMqtt/Interfaces/IEncodeHeader.h>
#include <cleanMqtt/Mqtt/Packets/ErrorCodes.h>

#include <cstdint>
#include <vector>
#include <type_traits>
#include <memory>
#include <cassert>

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
		{
			class BasePacket
			{
			public:
				DELETE_COPY_ASSIGNMENT_AND_CONSTRUCTOR(BasePacket)

				BasePacket() = delete;
				BasePacket(const FixedHeaderFlags& flags) noexcept;
				BasePacket(ByteBuffer&& dataBuffer) noexcept;
				BasePacket(BasePacket&& other) noexcept;
				virtual ~BasePacket();

				BasePacket& operator=(BasePacket&& other) noexcept;

				virtual PacketType getPacketType() const noexcept = 0;

				EncodeResult encode();
				DecodeResult decode();

				const FixedHeader& getFixedHeader() const;
				const ByteBuffer* getDataBuffer() const;

			protected:
				std::size_t calculateFixedHeaderRemainingLength() const;

				virtual void onFixedHeaderDecoded() const;

				void addEncodeHeader(const IEncodeHeader* header);
				void addDecodeHeader(IDecodeHeader* header);

			private:
				FixedHeader m_fixedHeader;

				std::vector<const IEncodeHeader*> m_otherEncodeHeaders;
				std::vector<IDecodeHeader*> m_otherDecodeHeaders;
				ByteBuffer* m_dataBuffer{ nullptr };
			};
		}
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETS_PACKET_H