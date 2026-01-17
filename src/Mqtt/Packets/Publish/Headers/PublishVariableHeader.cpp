#include <kmMqtt/Mqtt/Packets/Publish/Headers/PublishVariableHeader.h>

namespace kmMqtt
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

			if (qos >= Qos::QOS_1)
			{
				packetIdentifier = buffer.readUInt16();
			}

			return properties.decode(buffer);
		}

		void PublishVariableHeader::encode(ByteBuffer& buffer) const
		{
			std::string topicNameStr{ topicName.getString() };

			assert(!topicNameStr.empty());
			assert(topicNameStr.find("*", 0) == topicNameStr.npos);

			topicName.encode(buffer);
			if (qos >= Qos::QOS_1) buffer.append(packetIdentifier);

			properties.encode(buffer); //Size must be encoded even if no properties.
		}

		std::size_t PublishVariableHeader::getEncodedBytesSize() const noexcept
		{
			auto size{ topicName.encodingSize() };
			if (qos >= Qos::QOS_1) size += sizeof(packetIdentifier);

			size += properties.encodingSize(); //Size must be encoded even if no properties.

			return size;
		}
	}
}