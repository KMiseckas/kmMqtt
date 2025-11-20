#include <doctest.h>
#include <cleanMqtt/Mqtt/Params/ConnectAddress.h>

#include <cstring>

#define IP4_VAR(varName, hostname, port) \
static constexpr const char* varName =  #hostname ":" #port; \
static constexpr const char* varName##_hostname = #hostname; \
static constexpr const char* varName##_port = #port; \

#define IP6_VAR(varName, hostname, port) \
static constexpr const char* varName =  "[" #hostname "]:" #port; \
static constexpr const char* varName##_hostname = #hostname; \
static constexpr const char* varName##_port = #port; \

#define URL_VAR(varName, schema, hostname, port, path) \
static constexpr const char* varName =  schema "://" hostname ":" port path; \
static constexpr const char* varName##_hostname = hostname; \
static constexpr const char* varName##_port = port; \

TEST_SUITE("ConnectAddress Tests")
{
	using namespace cleanMqtt::mqtt;

	namespace
	{

		IP4_VAR(k_ipv4_1, 27.0.0.34, 65);
		IP6_VAR(k_ipv6_1, 2001:0db8:0000:0000:0000:8a2e:0370:7334, 123);
		IP6_VAR(k_ipv6_2, 2002:0db8:0000:0000:0000:8a2e:0370:7334, 200);
		URL_VAR(k_url_1, "wss", "www.url.com", "34", "/path/");
	}

	TEST_CASE("toAddress parsing")
	{
		SUBCASE("Singles")
		{
			auto addresses = Address::toAddress(k_ipv4_1);
			CHECK(addresses.size() == 1);
			CHECK(addresses[0].hostname() == k_ipv4_1_hostname);
			CHECK(addresses[0].port() == k_ipv4_1_port);
			CHECK(addresses[0].locatorType() == LocatorType::IP4);

			addresses = Address::toAddress(k_ipv6_1);
			CHECK(addresses.size() == 1);
			CHECK(addresses[0].hostname() == k_ipv6_1_hostname);
			CHECK(addresses[0].port() == k_ipv6_1_port);
			CHECK(addresses[0].locatorType() == LocatorType::IP6);

			addresses = Address::toAddress(k_url_1);
			CHECK(addresses.size() == 1);
			CHECK(addresses[0].hostname() == k_url_1_hostname);
			CHECK(addresses[0].port() == k_url_1_port);
			CHECK(addresses[0].locatorType() == LocatorType::HOSTNAME);
		}

		SUBCASE("Vectors")
		{
			std::string addressesTxt;
			addressesTxt.append(k_ipv4_1).append(" ")
				.append(k_ipv6_1).append(" ")
				.append(k_url_1);

			auto addresses = Address::toAddress(addressesTxt.c_str());
			CHECK(addresses.size() == 3);
			CHECK(addresses[0].hostname() == k_ipv4_1_hostname);
			CHECK(addresses[0].port() == k_ipv4_1_port);
			CHECK(addresses[0].locatorType() == LocatorType::IP4);
			CHECK(addresses[1].hostname() == k_ipv6_1_hostname);
			CHECK(addresses[1].port() == k_ipv6_1_port);
			CHECK(addresses[1].locatorType() == LocatorType::IP6);
			CHECK(addresses[2].hostname() == k_url_1_hostname);
			CHECK(addresses[2].port() == k_url_1_port);
			CHECK(addresses[2].locatorType() == LocatorType::HOSTNAME);
		}
	}

	TEST_CASE("Connect Address")
	{
		std::string addressesTxt;
		addressesTxt.append(k_ipv4_1).append(" ")
			.append(k_ipv6_1).append(" ")
			.append(k_url_1);

		auto addresses = Address::toAddress(addressesTxt.c_str());
		const auto temp{ addresses[0] };
		addresses.erase(addresses.begin());

		ConnectAddress connAddress{ temp, addresses };

		CHECK(connAddress.primaryAddress.hostname() == k_ipv4_1_hostname);
		CHECK(connAddress.otherAddresses.size() == 2);
		CHECK(connAddress.otherAddresses[0].hostname() == k_ipv6_1_hostname);
		CHECK(connAddress.otherAddresses[0].port() == k_ipv6_1_port);
		CHECK(connAddress.otherAddresses[0].locatorType() == LocatorType::IP6);
		CHECK(connAddress.otherAddresses[1].hostname() == k_url_1_hostname);
		CHECK(connAddress.otherAddresses[1].port() == k_url_1_port);
		CHECK(connAddress.otherAddresses[1].locatorType() == LocatorType::HOSTNAME);
	}

	TEST_CASE("ReconnectAddress Cycle Test")
	{
		std::string addressesTxt;
		addressesTxt.append(k_ipv4_1).append(" ")
			.append(k_ipv6_1).append(" ")
			.append(k_url_1);

		auto addresses = Address::toAddress(addressesTxt.c_str());
		const auto temp{ addresses[0] };
		addresses.erase(addresses.begin());

		ConnectAddress connAddress{ temp, addresses };

		ReconnectAddress reconnectAddress;
		reconnectAddress.reset(connAddress);

		CHECK(reconnectAddress.primaryAddress.hostname() == k_ipv4_1_hostname);
		CHECK(reconnectAddress.primaryAddress.port() == k_ipv4_1_port);
		CHECK(reconnectAddress.otherAddresses.size() == 2);
		CHECK(reconnectAddress.otherAddresses[0].hostname() == k_ipv6_1_hostname);
		CHECK(reconnectAddress.otherAddresses[0].port() == k_ipv6_1_port);
		CHECK(reconnectAddress.otherAddresses[0].locatorType() == LocatorType::IP6);
		CHECK(reconnectAddress.otherAddresses[1].hostname() == k_url_1_hostname);
		CHECK(reconnectAddress.otherAddresses[1].port() == k_url_1_port);
		CHECK(reconnectAddress.otherAddresses[1].locatorType() == LocatorType::HOSTNAME);
		CHECK(reconnectAddress.usedAddresses.size() == 0);

		SUBCASE("Cycling")
		{
			CHECK(reconnectAddress.tryCycleToNextPrimaryAddress() == true);
			CHECK(reconnectAddress.primaryAddress.hostname() == k_ipv6_1_hostname);
			CHECK(reconnectAddress.primaryAddress.port() == k_ipv6_1_port);
			CHECK(reconnectAddress.otherAddresses.size() == 1);
			CHECK(reconnectAddress.otherAddresses[0].hostname() == k_url_1_hostname);
			CHECK(reconnectAddress.otherAddresses[0].port() == k_url_1_port);
			CHECK(reconnectAddress.otherAddresses[0].locatorType() == LocatorType::HOSTNAME);
			CHECK(reconnectAddress.usedAddresses.size() == 1);
			CHECK(reconnectAddress.usedAddresses[0].hostname() == k_ipv4_1_hostname);
			CHECK(reconnectAddress.usedAddresses[0].port() == k_ipv4_1_port);

			CHECK(reconnectAddress.tryCycleToNextPrimaryAddress() == true);
			CHECK(reconnectAddress.primaryAddress.hostname() == k_url_1_hostname);
			CHECK(reconnectAddress.otherAddresses.size() == 0);
			CHECK(reconnectAddress.usedAddresses.size() == 2);
			CHECK(reconnectAddress.usedAddresses[0].hostname() == k_ipv4_1_hostname);
			CHECK(reconnectAddress.usedAddresses[0].port() == k_ipv4_1_port);
			CHECK(reconnectAddress.usedAddresses[1].hostname() == k_ipv6_1_hostname);
			CHECK(reconnectAddress.usedAddresses[1].port() == k_ipv6_1_port);

			CHECK(reconnectAddress.tryCycleToNextPrimaryAddress() == false);
			CHECK(reconnectAddress.primaryAddress.hostname() == k_url_1_hostname);
			CHECK(reconnectAddress.otherAddresses.size() == 0);
			CHECK(reconnectAddress.usedAddresses.size() == 2);
			CHECK(reconnectAddress.usedAddresses[0].hostname() == k_ipv4_1_hostname);
			CHECK(reconnectAddress.usedAddresses[1].hostname() == k_ipv6_1_hostname);

			reconnectAddress.reset();
			CHECK(reconnectAddress.otherAddresses.size() == 0);
			CHECK(reconnectAddress.usedAddresses.size() == 0);
		}

		SUBCASE("Adding")
		{
			std::vector<Address> addresses;
			addresses.push_back(Address::createIp6(k_ipv6_1_hostname, k_ipv6_1_port));
			addresses.push_back(Address::createIp6(k_ipv6_2_hostname, k_ipv6_2_port));

			reconnectAddress.addAddresses(addresses);

			CHECK(reconnectAddress.primaryAddress.hostname() == k_ipv4_1_hostname);
			CHECK(reconnectAddress.otherAddresses.size() == 3);
			CHECK(reconnectAddress.otherAddresses[2].hostname() == k_ipv6_2_hostname);
			CHECK(reconnectAddress.otherAddresses[2].locatorType() == LocatorType::IP6);
			CHECK(reconnectAddress.usedAddresses.size() == 0);

			CHECK(reconnectAddress.tryCycleToNextPrimaryAddress() == true);
			CHECK(reconnectAddress.tryCycleToNextPrimaryAddress() == true);

			CHECK(reconnectAddress.primaryAddress.hostname() == k_url_1_hostname);
			CHECK(reconnectAddress.otherAddresses.size() == 1);
			CHECK(reconnectAddress.otherAddresses[0].hostname() == k_ipv6_2_hostname);
			CHECK(reconnectAddress.otherAddresses[0].locatorType() == LocatorType::IP6);
			CHECK(reconnectAddress.usedAddresses.size() == 2);

			//Try test duplicates addition
			reconnectAddress.addAddresses(addresses);

			CHECK(reconnectAddress.primaryAddress.hostname() == k_url_1_hostname);
			CHECK(reconnectAddress.otherAddresses.size() == 1);
			CHECK(reconnectAddress.otherAddresses[0].hostname() == k_ipv6_2_hostname);
			CHECK(reconnectAddress.otherAddresses[0].locatorType() == LocatorType::IP6);
			CHECK(reconnectAddress.usedAddresses.size() == 2);

			addresses.clear();
			addresses.push_back(Address::createURL(k_url_1_hostname, k_url_1_port));
			addresses.push_back(Address::createURL(k_url_1_hostname, k_url_1_port));
			reconnectAddress.addAddresses(addresses);

			CHECK(reconnectAddress.primaryAddress.hostname() == k_url_1_hostname);
			CHECK(reconnectAddress.otherAddresses.size() == 1);
			CHECK(reconnectAddress.otherAddresses[0].hostname() == k_ipv6_2_hostname);
			CHECK(reconnectAddress.otherAddresses[0].locatorType() == LocatorType::IP6);
			CHECK(reconnectAddress.usedAddresses.size() == 2);
		}

	}
}