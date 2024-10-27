#ifndef INCLUDE_CLEANMQTT_INTERFACES_IRECEIVECLIENTPACKETHANDLER_H
#define INCLUDE_CLEANMQTT_INTERFACES_IRECEIVECLIENTPACKETHANDLER_H

#include "cleanMqtt/Mqtt/Packets/Connection/ConnectAck.h"
#include <cleanMqtt/Mqtt/Packets/Connection/Disconnect.h>
#include <cleanMqtt/Mqtt/Packets/Publish/Publish.h>

namespace cleanMqtt
{
	namespace interfaces
	{
		using namespace mqtt::packets;

		class IReceiveClientPacketHandler
		{
		public:
			IReceiveClientPacketHandler() noexcept = default;

			virtual void handleReceivedConnectAcknowledge(const ConnectAck& packet) = 0;
			virtual void handleReceivedDisconnect(const Disconnect& packet) = 0;
			virtual void handleReceivedPublish(const Publish& packet) = 0;
			virtual void handleReceivedPublishComplete() = 0;
			virtual void handleReceivedPublishReceived() = 0;
			virtual void handleReceivedPublishReleased() = 0;
			virtual void handleReceivedSubscribeAcknowledge() = 0;
			virtual void handleReceivedUnsubscribeAcknowledge() = 0;
			virtual void handleReceivedPingResponse() = 0;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_INTERFACES_IRECEIVECLIENTPACKETHANDLER_H 
