#ifndef INTERFACE_KMMQTT_MQTT_PACKETS_UNSUBSCRIBE_HEADERS_UNSUBSCRIBEACKPAYLOADHEADER_H
#define INTERFACE_KMMQTT_MQTT_PACKETS_UNSUBSCRIBE_HEADERS_UNSUBSCRIBEACKPAYLOADHEADER_H

#include <vector>
#include <cstdint>
#include <kmMqtt/Interfaces/IDecodeHeader.h>
#include <kmMqtt/Mqtt/Packets/UnSubscribe/Codes/UnSubAckReasonCode.h>

namespace kmMqtt
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

#endif // INTERFACE_KMMQTT_MQTT_PACKETS_UNSUBSCRIBE_HEADERS_UNSUBSCRIBEACKPAYLOADHEADER_H
