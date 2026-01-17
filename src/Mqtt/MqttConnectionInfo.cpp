// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include "kmMqtt/Mqtt/MqttConnectionInfo.h"

#include "kmMqtt/Mqtt/State/SessionState/SessionState.h"

namespace kmMqtt
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