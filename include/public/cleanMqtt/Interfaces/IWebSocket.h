#ifndef INCLUDE_CLEANMQTT_INTERFACES_IWEBSOCKET_H
#define INCLUDE_CLEANMQTT_INTERFACES_IWEBSOCKET_H

#include <cleanMqtt/GlobalMacros.h>
#include <cleanMqtt/ByteBuffer.h>
#include <cleanMqtt/Mqtt/Params/ConnectAddress.h>
#include <functional>

namespace cleanMqtt
{
	/**
	 * Callback invoked when the WebSocket connects.
	 */
	using OnConnectCallback = std::function<void(bool)>;

	/**
	 * Callback invoked when the WebSocket disconnects.
	 */
	using OnDisconnectCallback = std::function<void()>;

	/**
	 * Callback invoked when data is received over the WebSocket.
	 * The ByteBuffer is moved to avoid unnecessary copies.
	 */
	using OnRecvdCallback = std::function<void(ByteBuffer&&)>;

	/**
	 * Callback invoked when an error occurs on the WebSocket.
	 * The uint16_t parameter represents the error code.
	 */
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
		virtual bool connect(const mqtt::Address& address) noexcept = 0;

		/**
		 * @brief Send data over the WebSocket connection.
		 * Partial sends are managed internally by the SDK, socket implementation does not need to handle them.
		 * 
		 * @param data The data to send. Socket must manage its own partial sends.
		 * @return The number of bytes sent, or a negative error code on failure.
		 */
		virtual int send(const ByteBuffer& data) noexcept = 0;

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
