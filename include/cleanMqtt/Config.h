#ifndef INCLUDE_CLEANMQTT_CONFIG_H
#define INCLUDE_CLEANMQTT_CONFIG_H

namespace cleanMqtt
{
	struct Config
	{
		std::uint32_t connectTimeOutMS{ 15000U };

		std::uint32_t pingTimeOutMS{ 10000U };
		bool pingAlways{ true }; //Ping server always, regardless if a control packet has been sent within the keep alive time or keep alive is 0.
		std::uint32_t defaultPingInterval{ 15000U }; //Ping interval to use when keepAlive is 0 and pingAlways is true.
	};
}

#endif //INCLUDE_CLEANMQTT_CONFIG_H
