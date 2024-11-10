#include <doctest.h>
#include <cleanMqtt/Mqtt/Params/ConnectAddress.h>

#include <cstring>

TEST_SUITE("ConnectAddress Tests")
{
	using namespace cleanMqtt::mqtt;

	namespace
	{
		static constexpr const char* k_ipv4_1 = "27.0.0.34:65";
		static constexpr const char* k_ipv4_2 = "142.100.200.1";
		static constexpr const char* k_ipv6_1 = "2001:0db8:0000:0000:0000:8a2e:0370:7334";
		static constexpr const char* k_ipv6_2 = "2001:db8::8a2e:370:7334";
		static constexpr const char* k_ipv6_3 = "[2001:0db8:0000:0000:0000:8a2e:0370:7334]:123";
		static constexpr const char* k_url_1 = "mqtts://some.url.com";
		static constexpr const char* k_url_2 = "wss://www.url.com:34/path/";
		static constexpr const char* k_url_3 = "www.url.com";
	}

	TEST_CASE("toAddress parsing")
	{
		SUBCASE("Singles")
		{
			auto& addresses = Address::toAddress(k_ipv4_1);
			CHECK(addresses.size() == 1);
			CHECK(addresses[0].value() == k_ipv4_1);
			CHECK(addresses[0].locatorType() == LocatorType::IP4);

			addresses = Address::toAddress(k_ipv4_2);
			CHECK(addresses.size() == 1);
			CHECK(addresses[0].value() == k_ipv4_2);
			CHECK(addresses[0].locatorType() == LocatorType::IP4);

			addresses = Address::toAddress(k_ipv6_1);
			CHECK(addresses.size() == 1);
			CHECK(addresses[0].value() == k_ipv6_1);
			CHECK(addresses[0].locatorType() == LocatorType::IP6);

			addresses = Address::toAddress(k_ipv6_2);
			CHECK(addresses.size() == 1);
			CHECK(addresses[0].value() == k_ipv6_2);
			CHECK(addresses[0].locatorType() == LocatorType::IP6);

			addresses = Address::toAddress(k_ipv6_3);
			CHECK(addresses.size() == 1);
			CHECK(addresses[0].value() == k_ipv6_3);
			CHECK(addresses[0].locatorType() == LocatorType::IP6);

			addresses = Address::toAddress(k_url_1);
			CHECK(addresses.size() == 1);
			CHECK(addresses[0].value() == k_url_1);
			CHECK(addresses[0].locatorType() == LocatorType::HOSTNAME);

			addresses = Address::toAddress(k_url_2);
			CHECK(addresses.size() == 1);
			CHECK(addresses[0].value() == k_url_2);
			CHECK(addresses[0].locatorType() == LocatorType::HOSTNAME);

			addresses = Address::toAddress(k_url_3);
			CHECK(addresses.size() == 1);
			CHECK(addresses[0].value() == k_url_3);
			CHECK(addresses[0].locatorType() == LocatorType::HOSTNAME);
		}

		SUBCASE("Vectors")
		{
			std::string addressesTxt;
			addressesTxt.append(k_ipv4_1).append(" ")
				.append(k_ipv6_1).append(" ")
				.append(k_url_1);

			auto& addresses = Address::toAddress(addressesTxt.c_str());
			CHECK(addresses.size() == 3);
			CHECK(addresses[0].value() == k_ipv4_1);
			CHECK(addresses[0].locatorType() == LocatorType::IP4);
			CHECK(addresses[1].value() == k_ipv6_1);
			CHECK(addresses[1].locatorType() == LocatorType::IP6);
			CHECK(addresses[2].value() == k_url_1);
			CHECK(addresses[2].locatorType() == LocatorType::HOSTNAME);
		}
	}

	TEST_CASE("Connect Address")
	{
		std::string addressesTxt;
		addressesTxt.append(k_ipv4_1).append(" ")
			.append(k_ipv6_1).append(" ")
			.append(k_url_1);

		auto& addresses = Address::toAddress(addressesTxt.c_str());
		const auto temp{ addresses[0] };
		addresses.erase(addresses.begin());

		ConnectAddress connAddress{ temp, addresses };

		CHECK(connAddress.primaryAddress.value() == k_ipv4_1);
		CHECK(connAddress.otherAddresses.size() == 2);
		CHECK(connAddress.otherAddresses[0].value() == k_ipv6_1);
		CHECK(connAddress.otherAddresses[0].locatorType() == LocatorType::IP6);
		CHECK(connAddress.otherAddresses[1].value() == k_url_1);
		CHECK(connAddress.otherAddresses[1].locatorType() == LocatorType::HOSTNAME);
	}

	TEST_CASE("ReconnectAddress Cycle Test")
	{
		std::string addressesTxt;
		addressesTxt.append(k_ipv4_1).append(" ")
			.append(k_ipv6_1).append(" ")
			.append(k_url_1);

		auto& addresses = Address::toAddress(addressesTxt.c_str());
		const auto temp{ addresses[0] };
		addresses.erase(addresses.begin());

		ConnectAddress connAddress{ temp, addresses };

		ReconnectAddress reconnectAddress;
		reconnectAddress.reset(connAddress);

		CHECK(reconnectAddress.primaryAddress.value() == k_ipv4_1);
		CHECK(reconnectAddress.otherAddresses.size() == 2);
		CHECK(reconnectAddress.otherAddresses[0].value() == k_ipv6_1);
		CHECK(reconnectAddress.otherAddresses[0].locatorType() == LocatorType::IP6);
		CHECK(reconnectAddress.otherAddresses[1].value() == k_url_1);
		CHECK(reconnectAddress.otherAddresses[1].locatorType() == LocatorType::HOSTNAME);
		CHECK(reconnectAddress.usedAddresses.size() == 0);

		SUBCASE("Cycling")
		{
			CHECK(reconnectAddress.tryCycleToNextPrimaryAddress() == true);
			CHECK(reconnectAddress.primaryAddress.value() == k_ipv6_1);
			CHECK(reconnectAddress.otherAddresses.size() == 1);
			CHECK(reconnectAddress.otherAddresses[0].value() == k_url_1);
			CHECK(reconnectAddress.otherAddresses[0].locatorType() == LocatorType::HOSTNAME);
			CHECK(reconnectAddress.usedAddresses.size() == 1);
			CHECK(reconnectAddress.usedAddresses[0].value() == k_ipv4_1);

			CHECK(reconnectAddress.tryCycleToNextPrimaryAddress() == true);
			CHECK(reconnectAddress.primaryAddress.value() == k_url_1);
			CHECK(reconnectAddress.otherAddresses.size() == 0);
			CHECK(reconnectAddress.usedAddresses.size() == 2);
			CHECK(reconnectAddress.usedAddresses[0].value() == k_ipv4_1);
			CHECK(reconnectAddress.usedAddresses[1].value() == k_ipv6_1);

			CHECK(reconnectAddress.tryCycleToNextPrimaryAddress() == false);
			CHECK(reconnectAddress.primaryAddress.value() == k_url_1);
			CHECK(reconnectAddress.otherAddresses.size() == 0);
			CHECK(reconnectAddress.usedAddresses.size() == 2);
			CHECK(reconnectAddress.usedAddresses[0].value() == k_ipv4_1);
			CHECK(reconnectAddress.usedAddresses[1].value() == k_ipv6_1);

			reconnectAddress.reset();
			CHECK(reconnectAddress.otherAddresses.size() == 0);
			CHECK(reconnectAddress.usedAddresses.size() == 0);
		}

		SUBCASE("Adding")
		{
			std::vector<Address> addresses;
			addresses.push_back(Address::createIp6(k_ipv6_2));
			addresses.push_back(Address::createIp6(k_ipv6_3));

			reconnectAddress.addAddresses(addresses);

			CHECK(reconnectAddress.primaryAddress.value() == k_ipv4_1);
			CHECK(reconnectAddress.otherAddresses.size() == 4);
			CHECK(reconnectAddress.otherAddresses[2].value() == k_ipv6_2);
			CHECK(reconnectAddress.otherAddresses[2].locatorType() == LocatorType::IP6);
			CHECK(reconnectAddress.otherAddresses[3].value() == k_ipv6_3);
			CHECK(reconnectAddress.otherAddresses[3].locatorType() == LocatorType::IP6);
			CHECK(reconnectAddress.usedAddresses.size() == 0);

			CHECK(reconnectAddress.tryCycleToNextPrimaryAddress() == true);
			CHECK(reconnectAddress.tryCycleToNextPrimaryAddress() == true);
			CHECK(reconnectAddress.tryCycleToNextPrimaryAddress() == true);

			CHECK(reconnectAddress.primaryAddress.value() == k_ipv6_2);
			CHECK(reconnectAddress.otherAddresses.size() == 1);
			CHECK(reconnectAddress.otherAddresses[0].value() == k_ipv6_3);
			CHECK(reconnectAddress.otherAddresses[0].locatorType() == LocatorType::IP6);
			CHECK(reconnectAddress.usedAddresses.size() == 3);

			reconnectAddress.addAddresses(addresses);

			CHECK(reconnectAddress.primaryAddress.value() == k_ipv6_2);
			CHECK(reconnectAddress.otherAddresses.size() == 1);
			CHECK(reconnectAddress.otherAddresses[0].value() == k_ipv6_3);
			CHECK(reconnectAddress.otherAddresses[0].locatorType() == LocatorType::IP6);
			CHECK(reconnectAddress.usedAddresses.size() == 3);

			addresses.clear();
			addresses.push_back(Address::createURL(k_url_3));
			addresses.push_back(Address::createURL(k_url_1));
			reconnectAddress.addAddresses(addresses);

			CHECK(reconnectAddress.primaryAddress.value() == k_ipv6_2);
			CHECK(reconnectAddress.otherAddresses.size() == 2);
			CHECK(reconnectAddress.otherAddresses[0].value() == k_ipv6_3);
			CHECK(reconnectAddress.otherAddresses[0].locatorType() == LocatorType::IP6);
			CHECK(reconnectAddress.otherAddresses[1].value() == k_url_3);
			CHECK(reconnectAddress.otherAddresses[1].locatorType() == LocatorType::HOSTNAME);
			CHECK(reconnectAddress.usedAddresses.size() == 3);
		}

	}
}