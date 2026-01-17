// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_PACKETS_PACKET_H
#define INCLUDE_KMMQTT_MQTT_PACKETS_PACKET_H

#include <kmMqtt/ByteBuffer.h>
#include <kmMqtt/Mqtt/Packets/PacketUtils.h>
#include <kmMqtt/Mqtt/Packets/PacketType.h>
#include <kmMqtt/Mqtt/Packets/FixedHeader.h>
#include <kmMqtt/GlobalMacros.h>
#include <kmMqtt/Interfaces/IEncodeHeader.h>
#include <kmMqtt/Mqtt/Packets/ErrorCodes.h>

#include <cstdint>
#include <vector>
#include <type_traits>
#include <memory>
#include <cassert>

namespace kmMqtt
{
	namespace mqtt
	{
		/**
		 * @brief Base class for all MQTT packets.
		 * Provides common functionality for encoding and decoding packets,
		 * managing fixed headers, and handling data buffers.
		 */
		class BasePacket
		{
		public:
			DELETE_COPY_ASSIGNMENT_AND_CONSTRUCTOR(BasePacket)

			BasePacket();
			BasePacket(const FixedHeaderFlags& flags) noexcept;
			BasePacket(ByteBuffer&& dataBuffer) noexcept;
			BasePacket(BasePacket&& other) noexcept;
			virtual ~BasePacket();

			BasePacket& operator=(BasePacket&& other) noexcept;

			/**
			 * @brief Gets the packet type of the MQTT packet.
			 * 
			 * @return The PacketType enum value representing the packet type.
			 */
			virtual PacketType getPacketType() const noexcept = 0;

			/**
			 * @brief Encodes the MQTT packet into a ByteBuffer.
			 * 
			 * @return An EncodeResult indicating success or failure of the encoding process.
			 */
			EncodeResult encode();

			/**
			 * @brief Decodes the MQTT packet from the internal data buffer.
			 * 
			 * @return A DecodeResult indicating success or failure of the decoding process.
			 */
			DecodeResult decode();

			/**
			 * @brief Gets the fixed header of the MQTT packet.
			 * 
			 * @return A constant reference to the FixedHeader.
			 */
			const FixedHeader& getFixedHeader() const;

			/**
			 * @brief Gets the data buffer of the MQTT packet.
			 * 
			 * @return A constant reference to the ByteBuffer containing the packet data.
			 */
			const ByteBuffer& getDataBuffer() const;

			/**
			 * @brief Extracts the data buffer, transferring ownership to the caller.
			 * 
			 * @return A ByteBuffer containing the packet data.
			 */
			ByteBuffer&& extractDataBuffer() noexcept;

		protected:
			/**
			 * @brief Calculates the remaining length for the fixed header based on other headers.
			 * 
			 * @return The calculated remaining length as a size_t.
			 */
			std::size_t calculateFixedHeaderRemainingLength() const;

			/**
			 * @brief Callback invoked after the fixed header has been decoded.
			 * Can be overridden by derived classes for additional processing thats required post fixed header decoding.
			 */
			virtual void onFixedHeaderDecoded() const;

			/**
			 * @brief Adds an encode header to the packet. The base packet does not take ownership of the header.
			 * Addition order matters! These are encoded in the order they are added.
			 * @param header Pointer to the IEncodeHeader to add.
			 */
			void addEncodeHeader(const IEncodeHeader* header);

			/**
			 * @brief Adds a decode header to the packet. The base packet does not take ownership of the header.
			 * Addition order matters! These are decoded in the order they are added.
			 * @param header Pointer to the IDecodeHeader to add.
			 */
			void addDecodeHeader(IDecodeHeader* header);

		private:
			FixedHeader m_fixedHeader;

			std::vector<IDecodeHeader*> m_otherDecodeHeaders;
			std::vector<const IEncodeHeader*> m_otherEncodeHeaders;
			ByteBuffer m_dataBuffer;
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_PACKETS_PACKET_H