#ifndef INCLUDE_CLEANMQTT_INTERFACES_ISENDJOB_H
#define INCLUDE_CLEANMQTT_INTERFACES_ISENDJOB_H

#include <cleanMqtt/GlobalMacros.h>
#include <cleanMqtt/Mqtt/Packets/BasePacket.h>
#include <cleanMqtt/Mqtt/MqttConnectionInfo.h>
#include <cstdint>
#include <functional>
#include <cleanMqtt/Mqtt/Packets/ErrorCodes.h>

namespace cleanMqtt
{
	namespace interfaces
	{
#define PACKET_SIZE_POST_ENCODE(packet)\
		packet.getFixedHeader().getEncodedBytesSize() + packet.getFixedHeader().remainingLength.uint32Value()\


#define CHECK_ENFORCE_MAX_PACKET_SIZE(result, packetSize, allowedSize)\
		if (packetSize > allowedSize)\
		{\
			LogInfo("", "Packet is over the maximum allowed packet size that the broker accepts.");\
			return interfaces::SendResultData{ packetSize, false, interfaces::NoSendReason::OVER_MAX_PACKET_SIZE, result, -1};\
		}\

		enum class NoSendReason : std::uint8_t
		{
			NONE,
			SOCKET_SEND_ERROR,
			OVER_MAX_PACKET_SIZE,
			ENCODE_ERROR
		};

		struct PUBLIC_API SendResultData
		{
			SendResultData() noexcept = default;
			SendResultData(std::size_t size, bool sent, NoSendReason reason, mqtt::packets::EncodeResult encodeResult, int error = 0) noexcept
				: packetSize{ size }, wasSent{ sent }, noSendReason{ reason }, encodeResult{ encodeResult }, socketError {error}
			{
			}

			std::size_t packetSize{ 0U };
			bool wasSent{ false };
			NoSendReason noSendReason{ NoSendReason::NONE };
			mqtt::packets::EncodeResult encodeResult;
			int socketError{ 0 };
		};

		class PUBLIC_API ISendJob
		{
		public:
			DELETE_MOVE_ASSIGNMENT_AND_CONSTRUCTOR(ISendJob)
			DELETE_COPY_ASSIGNMENT_AND_CONSTRUCTOR(ISendJob)

			using PacketSendDelegate = std::function<int(const mqtt::packets::BasePacket& packet)>;

			ISendJob(mqtt::MqttConnectionInfo* connectionInfo,
				PacketSendDelegate sendPacketCallback) noexcept
				: m_mqttConnectionInfo{ std::move(connectionInfo) },
				m_packetSendCallback{ sendPacketCallback }
			{};
			virtual ~ISendJob() {};

			virtual SendResultData send() noexcept = 0;
			virtual void cancel() noexcept = 0;

		protected:
			PacketSendDelegate m_packetSendCallback;
			mqtt::MqttConnectionInfo* m_mqttConnectionInfo;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_INTERFACES_ISENDJOB_H
