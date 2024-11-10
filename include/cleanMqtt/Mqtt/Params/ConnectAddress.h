#ifndef INTERFACE_CLEANMQTT_MQTT_PARAMS_CONNECTADRESS_H
#define INTERFACE_CLEANMQTT_MQTT_PARAMS_CONNECTADRESS_H

#include "cleanMqtt/Utils/Utils.h"
#include <string>
#include <cstdint>
#include <vector>
#include <memory>

namespace cleanMqtt
{
	namespace mqtt
	{
		enum class LocatorType : std::uint8_t
		{
			UNKNOWN = 0U,
			IP4,
			IP6,
			HOSTNAME
		};

		struct Address
		{
			Address() = default;

			static Address createIp4(const char* ip) noexcept
			{
				return Address(ip, LocatorType::IP4);
			}

			static Address createIp6(const char* ip) noexcept
			{
				return Address(ip, LocatorType::IP6);
			}

			static Address createURL(const char* url) noexcept
			{
				return Address(url, LocatorType::HOSTNAME);
			}

			static std::vector<Address> toAddress(const char* targetAddress)
			{
				std::vector<Address> addresses;
				std::vector<std::string> tokens = splitByDelimiter(targetAddress, " ");

				for (const std::string& token : tokens)
				{
					const std::regex ipv4Regex(R"((\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})(?::(\d{1,5}))?)");
					const std::regex ipv6Regex_1(R"(([a-fA-F0-9:]+)(?::(\d{1,5}))?)");
					const std::regex ipv6Regex_2(R"(\[([a-fA-F0-9:]+)\](?::(\d{1,5}))?)");


					if (std::regex_match(token, ipv4Regex))
					{
						Address address{ token.c_str(), LocatorType::IP4 };
						addresses.push_back(address);
					}
					else if (std::regex_match(token, ipv6Regex_1) || std::regex_match(token, ipv6Regex_2))
					{
						Address address{ token.c_str(), LocatorType::IP6 };
						addresses.push_back(address);
					}
					else
					{
						Address address{ token.c_str(), LocatorType::HOSTNAME };
						addresses.push_back(address);
					}
				}

				return addresses;
			}

			inline LocatorType locatorType() noexcept
			{
				return m_locatorType;
			}

			inline const std::string& value() noexcept
			{
				return m_value;
			}

			bool operator==(const Address& other) const
			{
				if (&other == this)
				{
					return true;
				}

				if (other.m_locatorType != m_locatorType)
				{
					return false;
				}

				return other.m_value == m_value;
			}

		private:
			Address(const char* val, LocatorType type) noexcept
				: m_value{ val }, m_locatorType{ type }
			{
			}

			LocatorType m_locatorType{ LocatorType::UNKNOWN };
			std::string m_value{ "" };
		};

		struct ConnectAddress
		{
			ConnectAddress() noexcept
			{
				primaryAddress = Address::createURL("");
			}

			ConnectAddress(Address primary) noexcept
				: primaryAddress{ std::move(primary) }
			{
			}

			ConnectAddress(Address primary, std::vector<Address> other) noexcept
				: primaryAddress{std::move(primary)}, otherAddresses{std::move(other)}
			{
			}

			Address primaryAddress;
			std::vector<Address> otherAddresses;
		};

		struct ReconnectAddress : public ConnectAddress
		{
		public:
			bool tryCycleToNextPrimaryAddress() noexcept
			{
				if (otherAddresses.size() == 0)
				{
					return false;
				}

				usedAddresses.push_back(primaryAddress);
				primaryAddress = otherAddresses[0];
				otherAddresses.erase(otherAddresses.begin());

				return true;
			}

			void addAddresses(const std::vector<Address>& addresses) noexcept
			{
				for (const Address& address : addresses)
				{
					bool canAdd{ true };

					if (primaryAddress == address)
					{
						continue;
					}

					for (const Address& usedAddress : usedAddresses)
					{
						if (usedAddress == address)
						{
							canAdd = false;
						}
					}

					if (!canAdd)
					{
						continue;
					}

					for (const Address& currAddress : otherAddresses)
					{
						if (currAddress == address)
						{
							canAdd = false;
						}
					}

					if (canAdd)
					{
						otherAddresses.push_back(address);
					}
				}
			}

			void reset(const ConnectAddress& blueprint = {}) noexcept
			{
				primaryAddress = blueprint.primaryAddress;
				otherAddresses = blueprint.otherAddresses;
				usedAddresses.clear();
			}

			std::vector<Address> usedAddresses;
		};
	}
}

#endif //INTERFACE_CLEANMQTT_MQTT_PARAMS_CONNECTADRESS_H 