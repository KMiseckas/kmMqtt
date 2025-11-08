#ifndef INCLUDE_CLEANMQTT_INTERFACES_IWEBSOCKET_H
#define INCLUDE_CLEANMQTT_INTERFACES_IWEBSOCKET_H

#include <cleanMqtt/GlobalMacros.h>
#include <cleanMqtt/ByteBuffer.h>
#include <functional>

namespace cleanMqtt
{
	using OnConnectCallback = std::function<void(bool)>;
	using OnDisconnectCallback = std::function<void()>;
	using OnRecvdCallback = std::function<void(ByteBuffer&&)>;
	using OnErrorCallback = std::function<void(std::uint16_t)>;

	/**
	 * @brief Interface for a WebSocket connection.
	 */
	class PUBLIC_API IWebSocket
	{
	public:
		virtual ~IWebSocket()
		{
		};

		/**
		 * @brief Connect to a WebSocket server.
		 * @param hostname The hostname of the server.
		 * @param port The port of the server.
		 * @return True if the operation to request a connect proccessed correctly, false otherwise.
		 */
		virtual bool connect(const std::string& hostname, const std::string& port = "80") noexcept = 0;

		/**
		 * @brief Send data over the WebSocket connection.
		 * @param data The data to send. Socket must manage its own partial sends.
		 * @return True if the data was sent successfully, false otherwise.
		 */
		virtual bool send(const ByteBuffer& data) noexcept = 0;

		/**
		 * @brief Close the WebSocket connection.
		 * @return True if the operation to request a close proccessed correctly, false otherwise.
		 */
		virtual bool close() noexcept = 0;

		/**
		 * @brief SDK calls this periodically in-case the WebSocket implementation needs to perform any tick tasks.
		 */
		virtual void tick() noexcept = 0;

		/**
		 * @brief Check if the WebSocket is connected.
		 * @return True if connected, false otherwise.
		 */
		virtual bool isConnected() const noexcept = 0;

		/**
		 * @brief Get the last error code.
		 * @return The last error code.
		 */
		virtual int getLastError() const noexcept = 0;

		/**
		 * @brief Get the last close code and reason.
		 * @return The last close code and reason.
		 */
		virtual int getLastCloseCode() const noexcept = 0;

		/**
		 * @brief Get the last close reason.
		 * @return The last close reason.
		 */
		virtual const char* getLastCloseReason() const noexcept = 0;

		/**
		 * @brief Set the callback for when the WebSocket connects.
		 * @param callback The callback function.
		 */
		virtual void setOnConnectCallback(OnConnectCallback callback) noexcept = 0;

		/**
		 * @brief Set the callback for when the WebSocket disconnects.
		 * @param callback The callback function.
		 */
		virtual void setOnDisconnectCallback(OnDisconnectCallback callback) noexcept = 0;

		/**
		 * @brief Set the callback for when data is received over the WebSocket.
		 * @param callback The callback function.
		 */
		virtual void setOnRecvdCallback(OnRecvdCallback callback) noexcept = 0;

		/**
		 * @brief Set the callback for when an error occurs on the WebSocket.
		 * @param callback The callback function.
		 */
		virtual void setOnErrorCallback(OnErrorCallback callback) noexcept = 0;
	};
}

#endif //INCLUDE_CLEANMQTT_INTERFACES_IWEBSOCKET_H 
