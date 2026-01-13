#ifndef INCLUDE_CLEANMQTT_UTILS_UTILS_H
#define INCLUDE_CLEANMQTT_UTILS_UTILS_H

#include "cleanMqtt/Mqtt/Packets/DataTypes.h"
#include "cleanMqtt/ByteBuffer.h"
#include "cleanMqtt/Logger/Log.h"

#include <cstring>
#include <vector>
#include <regex>
#include <cassert>

namespace cleanMqtt
{
	/**
	 * Splits a string by a given delimiter using regex.
	 * 
	 * @param target The string to be split.
	 * @param delimiter The delimiter used for splitting the string.
	 * 
	 * @return A vector containing the split parts of the string.
	 */
	inline std::vector<std::string> splitByDelimiter(const std::string& target, const char* delimiter)
	{
		std::vector<std::string> tokens;

		std::regex del{ delimiter };
		std::sregex_token_iterator iter{ target.begin(), target.end(), del, -1 };
		std::sregex_token_iterator end;

		while (iter != end)
		{
			tokens.push_back(*iter);
			++iter;
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
	inline bool separateMqttPacketByteBuffers(const ByteBuffer& buffer, std::vector<ByteBuffer>& packets, std::size_t& leftOverPosition)
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

#endif //INCLUDE_CLEANMQTT_UTILS_UTILS_H 