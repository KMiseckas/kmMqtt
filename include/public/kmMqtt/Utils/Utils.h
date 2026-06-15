// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_UTILS_UTILS_H
#define INCLUDE_KMMQTT_UTILS_UTILS_H

#include "kmMqtt/Mqtt/Packets/DataTypes.h"
#include "kmMqtt/ByteBuffer.h"
#include "kmMqtt/Logger/Log.h"

#include <cstring>
#include <kmMqtt/STL/KmString.h>
#include <kmMqtt/STL/KmVector.h>
#include <cassert>

namespace kmMqtt
{
	/**
	 * Splits a string by a literal delimiter.
	 * 
	 * @param target The string to be split.
	 * @param delimiter The delimiter used for splitting the string.
	 * 
	 * @return A vector containing the split parts of the string.
	 */
	inline kmStd::vector<kmStd::string> splitByDelimiter(const kmStd::string& target, const char* delimiter)
	{
		kmStd::vector<kmStd::string> tokens;

		if (delimiter == nullptr || delimiter[0] == '\0')
		{
			tokens.push_back(target);
			return tokens;
		}

		const kmStd::string delimiterStr{ delimiter };
		std::size_t tokenStart{ 0U };

		while (tokenStart <= target.size())
		{
			const std::size_t tokenEnd{ target.find(delimiterStr, tokenStart) };

			if (tokenEnd == kmStd::string::npos)
			{
				tokens.push_back(target.substr(tokenStart));
				break;
			}

			tokens.push_back(target.substr(tokenStart, tokenEnd - tokenStart));
			tokenStart = tokenEnd + delimiterStr.size();
		}

		return tokens;
	}

	/**
	 * Splits a ByteBuffer into multiple packets based on the MQTT packet structure.
	 * 
	 * @param buffer The ByteBuffer containing the MQTT packets.
	 * @param packets A vector to store the separated packets.
	 * @param leftOverPosition A position in buffer from which left over bytes begin.
	 * 
	 * @return Returns true if the separation was successful, false if the buffer does not contain complete packets or failed separation.
	 */
	inline bool separateMqttPacketByteBuffers(const ByteBuffer& buffer, kmStd::vector<ByteBuffer>& packets, std::size_t& leftOverPosition)
	{
		packets.clear();
		leftOverPosition = 0;

		size_t packetStart{ 0 };
		while (packetStart < buffer.size())
		{
			// If there are not enough bytes left to read a full packet header, break out of the loop.
			if (buffer.size() - buffer.readCursor() < 2)
			{
				return !packets.empty();;
			}

			buffer.resetReadCursor();
			assert(buffer.readCursor() == 0);

			buffer.incrementReadCursor(packetStart + 1);//Ignore the first byte (packet type and flags).
			mqtt::VariableByteInteger remainingLength{ mqtt::VariableByteInteger::tryCreateFromBuffer(buffer) };

			//Full packet size = Remaining Length + 1 byte Fixed Header + Remaining Length field size.
			size_t packetSize{ remainingLength.uint32Value() + sizeof(std::uint8_t) + remainingLength.encodingSize() };

			if (buffer.size() - packetStart < packetSize)
			{
				//Not enough bytes for the full packet.
				leftOverPosition = buffer.size() - packetStart;
				return !packets.empty();
			}

			ByteBuffer packetBuffer(packetSize);

			try
			{
				packetBuffer.append(buffer.bytes() + packetStart, packetSize);
			}
			catch (const std::exception& e)
			{
				LogError("Utils", "separateMqttPacketByteBuffers(), Failed to separate MQTT packets: %s", e.what());
				return false;
			}

			packets.push_back(std::move(packetBuffer));

			packetStart += packetSize;
		}

		return !packets.empty();
	}
}

#endif //INCLUDE_KMMQTT_UTILS_UTILS_H
