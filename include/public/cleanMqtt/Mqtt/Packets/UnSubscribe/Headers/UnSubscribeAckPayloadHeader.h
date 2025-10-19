#ifndef INTERFACE_CLEANMQTT_MQTT_PACKETS_UNSUBSCRIBE_HEADERS_UNSUBSCRIBEACKPAYLOADHEADER_H
#define INTERFACE_CLEANMQTT_MQTT_PACKETS_UNSUBSCRIBE_HEADERS_UNSUBSCRIBEACKPAYLOADHEADER_H

#include <vector>
#include <cstdint>
#include <cleanMqtt/Interfaces/IDecodeHeader.h>
#include <cleanMqtt/Mqtt/Packets/UnSubscribe/Codes/UnSubAckReasonCode.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		struct UnSubscribeAckPayloadHeader : public IDecodeHeader
		{
			UnSubscribeAckPayloadHeader() noexcept = default;

			DecodeResult decode(const ByteBuffer& buffer) noexcept override;

			std::vector<UnSubAckReasonCode> reasonCodes;
		};
	}
}

#endif // INTERFACE_CLEANMQTT_MQTT_PACKETS_UNSUBSCRIBE_HEADERS_UNSUBSCRIBEACKPAYLOADHEADER_H
