#ifndef INCLUDE_CLEANMQTT_MQTT_RECEIVEQUEUE_H
#define INCLUDE_CLEANMQTT_MQTT_RECEIVEQUEUE_H

#include "cleanMqtt/GlobalMacros.h"
#include "cleanMqtt/ByteBuffer.h"
#include "cleanMqtt/Interfaces/IReceiveClientPacketHandler.h"

#include <queue>
#include <mutex>

namespace cleanMqtt
{
	namespace mqtt
	{
		class ReceiveQueue
		{
		public:
			DELETE_COPY_ASSIGNMENT_AND_CONSTRUCTOR(ReceiveQueue)
			DELETE_MOVE_ASSIGNMENT_AND_CONSTRUCTOR(ReceiveQueue)

			ReceiveQueue() = delete;
			ReceiveQueue(interfaces::IReceiveClientPacketHandler* packetHandler) noexcept;
			~ReceiveQueue();

			void addToQueue(ByteBuffer&& byteBuffer);
			void receiveAvailablePackets();
			void clearQueue();

		private:
			bool tryAddFailedResult(packets::DecodeResult&& result) noexcept;

			std::queue<ByteBuffer> m_queuedData;
			interfaces::IReceiveClientPacketHandler* m_handler{ nullptr };
			std::mutex m_mutex;
			std::vector<packets::DecodeResult> m_failedDecodeResults;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_RECEIVEQUEUE_H 
