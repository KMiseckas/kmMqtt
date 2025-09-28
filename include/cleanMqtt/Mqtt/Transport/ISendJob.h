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
#define PACKET_SIZE_POST_ENCODE(packet)\
		packet.getFixedHeader().getEncodedBytesSize() + packet.getFixedHeader().remainingLength.uint32Value()\


#define CHECK_ENFORCE_MAX_PACKET_SIZE(result, packetSize, allowedSize)\
		if (packetSize > allowedSize)\
		{\
			LogInfo("", "Packet is over the maximum allowed packet size that the broker accepts.");\
			return mqtt::SendResultData{ packetSize, false, NoSendReason::OVER_MAX_PACKET_SIZE, result, -1};\
		}\

		class ISendJob
		{
		public:
			DELETE_MOVE_ASSIGNMENT_AND_CONSTRUCTOR(ISendJob)
			DELETE_COPY_ASSIGNMENT_AND_CONSTRUCTOR(ISendJob)

			using PacketSendDelegate = std::function<int(const packets::BasePacket& packet)>;

			ISendJob(MqttConnectionInfo* connectionInfo,
				PacketSendDelegate sendPacketCallback) noexcept
				: m_mqttConnectionInfo{ std::move(connectionInfo) },
				m_packetSendCallback{ sendPacketCallback }
			{};
			virtual ~ISendJob() {};

			virtual SendResultData send() noexcept = 0;
			virtual void cancel() noexcept = 0;

		protected:
			PacketSendDelegate m_packetSendCallback;
			MqttConnectionInfo* m_mqttConnectionInfo;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_INTERFACES_ISENDJOB_H
