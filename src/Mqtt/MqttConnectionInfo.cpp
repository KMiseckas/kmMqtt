#include "cleanMqtt/Mqtt/MqttConnectionInfo.h"

#include "cleanMqtt/Mqtt/State/SessionState/SessionState.h"

namespace cleanMqtt
{
	namespace mqtt
	{
		void MqttConnectionInfo::clear(bool clearSessionState) noexcept
		{
			connectArgs = ConnectArgs{ "CLIENT ID" };
			connectAddress = ConnectAddress{};
			reconnectAddress = ReconnectAddress{};
			hasBeenConnected = false;
			awaitingPingResponse = false;
			connectionStartTime = TimePoint{};
			lastControlPacketTime = TimePoint{};
			lastPingReqSentTime = TimePoint{};
			pingInterval = Milliseconds{ 0U };
			maxServerTopicAlias = 0U;
			maxServerPacketSize = MAX_PACKET_SIZE;
			serverKeepAlive = 0U;
			isRetainAvailable = true;
			if (clearSessionState)
			{
				sessionState.clear();
			}
			topicAliases = {};
			pendingSubscriptions.clear();
		}
	}
}