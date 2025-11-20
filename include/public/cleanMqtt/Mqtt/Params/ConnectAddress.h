#ifndef INTERFACE_CLEANMQTT_MQTT_PARAMS_CONNECTADRESS_H
#define INTERFACE_CLEANMQTT_MQTT_PARAMS_CONNECTADRESS_H

#include "cleanMqtt/Utils/Utils.h"
#include <cleanMqtt/Mqtt/Enums/LocatorType.h>
#include <string>
#include <cstdint>
#include <vector>
#include <memory>

namespace cleanMqtt
{
	namespace mqtt
	{
		struct Address
		{
			Address() = default;

			static Address createIp4(const char* ip, const char* port) noexcept
			{
				return Address(ip, port, LocatorType::IP4);
			}

			static Address createIp6(const char* ip, const char* port) noexcept
			{
				return Address(ip, port, LocatorType::IP6);
			}

			static Address createURL(const char* hostname, const char* port) noexcept
			{
				return Address(hostname, port, LocatorType::HOSTNAME);
			}

			static std::vector<Address> toAddress(const char* targetAddress)
			{
				std::vector<Address> addresses;
				std::vector<std::string> tokens = splitByDelimiter(targetAddress, " ");

				for (const std::string& token : tokens)
				{
					const std::regex ipv4Regex(R"((\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})(?::(\d{1,5}))?)");
					const std::regex ipv6Regex(R"(\[([a-fA-F0-9:]+)\](?::(\d{1,5}))?)");

					std::smatch result;

					if (std::regex_match(token, result, ipv4Regex))
					{
						if (result.size() < 3)
						{
							return addresses;
						}

						Address address{ result[1].str().c_str(), result[2].str().c_str(), LocatorType::IP4};
						addresses.push_back(address);
					}
					else if (std::regex_match(token, result, ipv6Regex))
					{
						if (result.size() < 3)
						{
							return addresses;
						}

						Address address{ result[1].str().c_str(), result[2].str().c_str(), LocatorType::IP6 };
						addresses.push_back(address);
					}
					else
					{
						std::string hostname;
						std::string port;

						if (tryParseHostnameAddress(token, hostname, port))
						{
							Address address{ hostname.c_str(), port.c_str(), LocatorType::HOSTNAME };
							addresses.push_back(address);
						}
					}
				}

				return addresses;
			}

			static bool tryParseHostnameAddress(const std::string& url, std::string& hostname, std::string& port)
			{
				static constexpr const char* schemeSeperator{ "://" };
				static constexpr std::size_t schemeSeperatorLen{ 3 };

				hostname = "";
				port = "";

				std::size_t schemeEnd = url.find(schemeSeperator);
				if (schemeEnd == std::string::npos)
				{
					//No scheme
					schemeEnd = 0;
				}

				std::size_t hostStart = schemeEnd == 0 ? 0 : schemeEnd + schemeSeperatorLen;
				std::size_t portStart = url.find(':', hostStart);
				std::size_t pathStart = url.find('/', hostStart);

				if (portStart != std::string::npos && (pathStart == std::string::npos || portStart < pathStart))
				{
					hostname = url.substr(hostStart, portStart - hostStart);
					std::size_t portEnd = (pathStart != std::string::npos) ? pathStart : url.size();

					port = url.substr(portStart + 1, portEnd - portStart - 1);
				}
				else
				{
					return false;
				}

				return true;
			}

			inline LocatorType locatorType() const noexcept
			{
				return m_locatorType;
			}

			inline const std::string& hostname() const noexcept
			{
				return m_hostname;
			}

			inline const std::string& port() const noexcept
			{
				return m_port;
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

				return other.m_hostname == m_hostname && other.m_port == m_port;
			}

		private:
			Address(const char* val, const char* port, LocatorType type) noexcept
				: m_hostname{ val },
				m_port{ port },
				m_locatorType {type}
			{
			}

			std::string m_hostname{ "" };
			std::string m_port{ "" };
			LocatorType m_locatorType{ LocatorType::UNKNOWN };
		};

		struct ConnectAddress
		{
			ConnectAddress() noexcept
			{
				primaryAddress = Address::createURL("", "80");
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