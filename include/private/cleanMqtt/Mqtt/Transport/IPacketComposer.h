#ifndef INCLUDE_CLEANMQTT_INTERFACES_ISENDJOB_H
#define INCLUDE_CLEANMQTT_INTERFACES_ISENDJOB_H

#include <cleanMqtt/GlobalMacros.h>
#include <cleanMqtt/Mqtt/Packets/BasePacket.h>
#include <cleanMqtt/Mqtt/MqttConnectionInfo.h>
#include <cstdint>
#include <functional>
#include <cleanMqtt/Mqtt/Packets/ErrorCodes.h>
#include <cleanMqtt/Mqtt/Transport/SendResultData.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		struct ComposeResult
		{
			DELETE_COPY_ASSIGNMENT_AND_CONSTRUCTOR(ComposeResult);

			ComposeResult(const EncodeResult& result, const ByteBuffer* data) noexcept
				: encodeResult{ result }
			{
				if (data != nullptr)
				{
					encodedData = ByteBuffer(*data);
				}
			}

			ComposeResult(ComposeResult&& other) noexcept
				: encodeResult{ other.encodeResult }, encodedData{ std::move(other.encodedData) }
			{
			}

			ComposeResult& operator=(ComposeResult&& other) noexcept
			{
				if (this != &other)
				{
					encodeResult = other.encodeResult;
					encodedData = std::move(other.encodedData);
				}
				return *this;
			}

			ComposeResult() = delete;

			EncodeResult encodeResult;
			ByteBuffer encodedData;
		};

		class IPacketComposer
		{
		public:
			DELETE_MOVE_ASSIGNMENT_AND_CONSTRUCTOR(IPacketComposer)
			DELETE_COPY_ASSIGNMENT_AND_CONSTRUCTOR(IPacketComposer)

			IPacketComposer(MqttConnectionInfo* connectionInfo) noexcept
				: m_mqttConnectionInfo{ std::move(connectionInfo) }
			{
			};
			virtual ~IPacketComposer() {};

			virtual ComposeResult compose() noexcept = 0;
			virtual void cancel() noexcept = 0;

		protected:
			MqttConnectionInfo* m_mqttConnectionInfo;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_INTERFACES_ISENDJOB_H
