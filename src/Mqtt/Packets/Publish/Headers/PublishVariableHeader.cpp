#include <cleanMqtt/Mqtt/Packets/Publish/Headers/PublishVariableHeader.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		PublishVariableHeader::PublishVariableHeader() noexcept
		{
		}

		PublishVariableHeader::PublishVariableHeader(const char* topicName,
			std::uint16_t packetId,
			Properties&& properties,
			Qos publishQOS) noexcept
			: topicName{ topicName },
			packetIdentifier{ packetId },
			properties{ std::move(properties) },
			qos{ publishQOS }
		{
		}

		DecodeResult PublishVariableHeader::decode(const ByteBuffer& buffer) noexcept
		{
			DecodeResult result;

			topicName.decode(buffer);

			result = std::move(properties.decode(buffer));

			if (qos >= Qos::QOS_1)
			{
				packetIdentifier = buffer.readUInt16();
			}

			properties.decode(buffer);

			return result;
		}

		void PublishVariableHeader::encode(ByteBuffer& buffer) const
		{
			std::string topicNameStr{ topicName.getString() };

			assert(!topicNameStr.empty());
			assert(topicNameStr.find("*", 0) == topicNameStr.npos);

			topicName.encode(buffer);
			if (qos >= Qos::QOS_1) buffer.append(packetIdentifier);
			if (properties.size() > 0) properties.encode(buffer);
		}

		std::size_t PublishVariableHeader::getEncodedBytesSize() const noexcept
		{
			auto size{ topicName.encodingSize() };
			if (qos >= Qos::QOS_1) size += sizeof(packetIdentifier);
			if (properties.size() > 0) size += properties.encodingSize();

			return size;
		}
	}
}