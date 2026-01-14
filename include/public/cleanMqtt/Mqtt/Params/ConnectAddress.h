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

			/**
			 * @brief Constructs an Address object from IP4.
			 * @param scheme The scheme of the address (e.g., "mqtt", "ws").
			 * @param ip The IP address.
			 * @param port The port number.
			 * @param path The path component of the address.
			 * 
			 * @return Address object.
			 */
			static Address createIp4(const char* scheme, const char* ip, const char* port, const char* path) noexcept
			{
				return Address(scheme, ip, port, path, LocatorType::IP4);
			}

			/**
			 * @brief Constructs an Address object from IP6.
			 * @param scheme The scheme of the address (e.g., "mqtt", "ws").
			 * @param ip The IP address.
			 * @param port The port number.
			 * @param path The path component of the address.
			 * 
			 * @return Address object.
			 */
			static Address createIp6(const char* scheme, const char* ip, const char* port, const char* path) noexcept
			{
				return Address(scheme, ip, port, path, LocatorType::IP6);
			}

			/**
			 * @brief Constructs an Address object from hostname.
			 * @param scheme The scheme of the address (e.g., "mqtt", "ws").
			 * @param hostname The hostname.
			 * @param port The port number.
			 * @param path The path component of the address.
			 * 
			 * @return Address object.
			 */
			static Address createURL(const char* scheme, const char* hostname, const char* port, const char* path) noexcept
			{
				return Address(scheme, hostname, port, path, LocatorType::HOSTNAME);
			}

			/**
			 * @brief Parses a space-separated list of URLs into a vector of Address objects. Supports IP4, IP6, and hostname formats.
			 * @param urls A space-separated string of URLs.
			 * 
			 * @return vector of address objects.
			 */
			static std::vector<Address> toAddress(const char* urls)
			{
				if (urls == nullptr || urls[0] == '\0')
				{
					return std::vector<Address>();
				}

				std::vector<Address> addresses;
				std::vector<std::string> tokens = splitByDelimiter(urls, " ");

				for (const std::string& token : tokens)
				{
					//Check if token has a scheme
					std::string scheme;
					std::string addressPart = token;
					std::string path;

					static constexpr const char* schemeSeperator{ "://" };
					static constexpr std::size_t schemeSeperatorLen{ 3 };
					
					std::size_t schemeEnd{ token.find(schemeSeperator) };
					if (schemeEnd != std::string::npos)
					{
						scheme = token.substr(0, schemeEnd);
						addressPart = token.substr(schemeEnd + schemeSeperatorLen);
					}

					//Get path
					std::size_t pathStart = addressPart.find('/');
					if (pathStart != std::string::npos)
					{
						path = addressPart.substr(pathStart + 1); //Skip the first '/' character
						addressPart = addressPart.substr(0, pathStart);
					}

					const std::regex ipv4Regex(R"((\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})(?::(\d{1,5}))?)");
					const std::regex ipv6Regex(R"(\[([a-fA-F0-9:]+)\](?::(\d{1,5}))?)");

					std::smatch result;

					if (std::regex_match(addressPart, result, ipv4Regex))
					{
						if (result.size() < 3)
						{
							return addresses;
						}

						Address address = Address::createIp4(scheme.c_str(), result[1].str().c_str(), result[2].str().c_str(), path.c_str());
						addresses.push_back(address);
					}
					else if (std::regex_match(addressPart, result, ipv6Regex))
					{
						if (result.size() < 3)
						{
							return addresses;
						}

						Address address = Address::createIp6(scheme.c_str(), result[1].str().c_str(), result[2].str().c_str(), path.c_str());
						addresses.push_back(address);
					}
					else
					{
						std::string hostname;
						std::string port;

						if (tryParseHostnameAddress(token, hostname, port))
						{
							Address address = Address::createURL(scheme.c_str(), hostname.c_str(), port.c_str(), path.c_str());
							addresses.push_back(address);
						}
					}
				}

				return addresses;
			}

			/**
			 * @brief Tries to parse a hostname address from a URL string.
			 * @param url The URL string to parse.
			 * @param hostname Output parameter for the parsed hostname.
			 * @param port Output parameter for the parsed port.
			 * 
			 * @return true if parsing was successful, false otherwise.
			 */
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

			inline const std::string& scheme() const noexcept
			{
				return m_scheme;
			}

			inline const std::string& path() const noexcept
			{
				return m_path;
			}

			inline std::string url() const noexcept
			{
				std::string urlStr;
				if (!m_scheme.empty())
				{
					urlStr += m_scheme + "://";
				}
				urlStr += m_hostname;
				if (!m_port.empty())
				{
					urlStr += ":" + m_port;
				}
				if (!m_path.empty())
				{
					urlStr += "/" + m_path;
				}
				return urlStr;
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

				return other.m_scheme == m_scheme && other.m_hostname == m_hostname && other.m_port == m_port && other.m_path == m_path;
			}

		private:
			Address(const char* scheme, const char* val, const char* port, const char* path, LocatorType type) noexcept
				: m_hostname{ val },
				m_port{ port },
				m_scheme{ scheme },
				m_path{ path },
				m_locatorType {type}
			{
			}

			std::string m_hostname{ "" };
			std::string m_port{ "" };
			std::string m_scheme{ "" };
			std::string m_path{ "" };
			LocatorType m_locatorType{ LocatorType::UNKNOWN };
		};

		/**
		 * @brief Structure representing a connect address with a primary address and optional other addresses.
		 * - Other addresses will be used for reconnection attempts when primary address fails.
		 */
		struct ConnectAddress
		{
			ConnectAddress() noexcept
			{
				primaryAddress = Address::createURL("", "", "80", "");
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

		/**
		 * @brief Structure representing a reconnect address that extends ConnectAddress.
		 * - Keeps track of used addresses and allows cycling through primary and other addresses.
		 */
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