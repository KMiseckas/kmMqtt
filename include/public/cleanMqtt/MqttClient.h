#pragma once

#include "cleanMqtt/MqttClientOptions.h"
#include "cleanMqtt/GlobalMacros.h"
#include "cleanMqtt/Config.h"
#include "cleanMqtt/Interfaces/IMqttEnvironment.h"
#include "cleanMqtt/Interfaces/IWebSocket.h"
#include "cleanMqtt/Environments/DefaultEnvironmentFactory.h"
#include "cleanMqtt/Mqtt/ClientError.h"
#include "cleanMqtt/Mqtt/ReqResult.h"
#include "cleanMqtt/Mqtt/MqttConnectionInfo.h"
#include "cleanMqtt/Mqtt/MqttClientEvents.h"
#include "cleanMqtt/Mqtt/Enums/ConnectionStatus.h"
#include "cleanMqtt/Mqtt/Enums/ReconnectionStatus.h"
#include "cleanMqtt/Mqtt/Params/ConnectArgs.h"
#include "cleanMqtt/Mqtt/Params/DisconnectArgs.h"
#include "cleanMqtt/Mqtt/Params/PublishOptions.h"
#include "cleanMqtt/Mqtt/Params/SubscribeOptions.h"
#include "cleanMqtt/Mqtt/Params/UnSubscribeOptions.h"
#include "cleanMqtt/Mqtt/Params/Topic.h"

#include <memory>

namespace cleanMqtt
{
	namespace mqtt
	{
		class MqttClientImpl;

		/**
		* @class MqttClient
		* @brief A high-level MQTT client interface for managing MQTT connections, publishing, subscribing, and handling events.
		*
		* This class provides an abstraction for interacting with an MQTT broker. It supports connecting to the broker,
		* publishing messages, subscribing to topics, and handling various MQTT events. The implementation details are
		* encapsulated in the MqttClientImpl class.
		*
		* @note This class is non-copyable and non-movable.
		*/
		class PUBLIC_API MqttClient
		{
		public:
			DELETE_COPY_ASSIGNMENT_AND_CONSTRUCTOR(MqttClient)
			DELETE_MOVE_ASSIGNMENT_AND_CONSTRUCTOR(MqttClient)

			/**
			* @brief Constructs a default MqttClient instance.
			*
			* This constructor initializes the MQTT client with a default environment and options.
			* It creates an instance of MqttClientImpl and manages its lifecycle.
			*/
			MqttClient();

			/**
			* @brief Constructs an MqttClient instance with a custom environment and options.
			* This constructor allows the user to specify a custom environment and options for the MQTT client.
			* If the client options specify asynchronous ticking, the client will start ticking asynchronously.
			*
			* @param env A pointer to the custom MQTT environment.
			* @param clientOptions The options for configuring the MQTT client.
			*/
			MqttClient(const IMqttEnvironment* const env, const MqttClientOptions& clientOptions);

			/**
			 * @brief Destructor for the MqttClient class.
			 * Cleans up resources and ensures proper shutdown of the MQTT client.
			 * Safe to call without shutdown() first.
			 */
			~MqttClient();

			/**
			 * @brief Connects to the MQTT broker using the provided connection arguments and address.
			 * 
			 * @param args The connection arguments for establishing the MQTT connection.
			 * @param address The address of the MQTT broker to connect to.
			 * 
			 * @return ReqResult indicating the result of the connection attempt.
			 */
			ReqResult connect(ConnectArgs&& args, ConnectAddress&& address) noexcept;

			/**
			 * @brief Publishes a message to a specified topic with the given payload and options.
			 * 
			 * @param topic The topic to which the message will be published.
			 * @param payload The payload of the message to be published.
			 * @param options The options for publishing the message.
			 * 
			 * @return ReqResult indicating the result of the publish attempt.
			 */
			ReqResult publish(const char* topic, ByteBuffer&& payload, PublishOptions&& options) noexcept;

			/**
			 * @brief Subscribe to the specified topics with the given subscribe options.
			 * 
			 * @param topics The topics to which the client is subscribing.
			 * @param options The subscribe options.
			 * 
			 * @return ReqResult indicating the result of the subscribe attempt.
			 */
			ReqResult subscribe(const std::vector<Topic>& topics, SubscribeOptions&& options) noexcept;

			/**
			 * @brief Unsubscribe from the specified topics with the given unsubscribe options.
			 * 
			 * @param topics The topics from which the client is unsubscribing.
			 * @param options The unsubscribe options.
			 * 
			 * @return ReqResult indicating the result of the unsubscribe attempt.
			 */
			ReqResult unSubscribe(const std::vector<Topic>& topics, UnSubscribeOptions&& options) noexcept;

			/**
			 * @brief Disconnects from the MQTT broker using the provided disconnect arguments.
			 * 
			 * @param args The disconnect arguments for terminating the MQTT connection.
			 * 
			 * @return ReqResult indicating the result of the disconnect attempt.
			 */
			ReqResult disconnect(DisconnectArgs&& args = {}) noexcept;

			/**
			 * @brief Shuts down the MQTT client, releasing all resources and stopping any ongoing operations.
			 * Use this for both in synchronous and asynchronous modes.
			 * 
			 * @return ClientError indicating the result of the shutdown operation.
			 */
			ClientError shutdown() noexcept;

			/**
			 * @brief Ticks the MQTT client to process incoming and outgoing messages and events.
			 * Use this method in synchronous mode to keep the client responsive.
			 * It's an error to call this method if the client is configured for asynchronous ticking.
			 * 
			 * @return ClientError indicating the result of the tick operation.
			 */
			ClientError tick() noexcept;

			/**
			 * @brief Accessor for the ErrorEvent.
			 * Invoked when an error occurs within the MQTT client.
			 * 
			 * @return Reference to the ErrorEvent instance.
			 */
			ErrorEvent& onErrorEvent() noexcept;

			/**
			 * @brief Accessor for the ConnectEvent.
			 * Invoked when the client successfully connects to the MQTT broker.
			 * Invoked when the client fails to connect to the MQTT broker on initial `connect` call.
			 * Contains ConnectAck information if available.
			 * 
			 * @return Reference to the ConnectEvent instance.
			 */
			ConnectEvent& onConnectEvent() noexcept;

			/**
			 * @brief Accessor for the DisconnectEvent.
			 * Invoked when the client disconnects from the MQTT broker.
			 * Invoked when the client fails to reconnect to the MQTT broker after an unexpected disconnection.
			 * Is not invoked as part of `connect` failure handling.
			 * Is not invoked as part of reconnection handling unless reconnection fails.
			 * 
			 * @return Reference to the DisconnectEvent instance.
			 */
			DisconnectEvent& onDisconnectEvent() noexcept;

			/**
			 * @brief Accessor for the ReconnectEvent.
			 * Invoked when the client successfully reconnects to the MQTT broker after an unexpected disconnection.
			 * Invoked when reconnection starts after an unexpected disconnection.
			 * 
			 * @return Reference to the ReconnectEvent instance.
			 */
			ReconnectEvent& onReconnectEvent() noexcept;

			/**
			 * @brief Accessor for the PublishEvent (receiving publish message from broker).
			 * Invoked when a publish message is received for a subscribed topic.
			 * 
			 * @return Reference to the PublishEvent instance.
			 */
			PublishEvent& onPublishEvent() noexcept;

			/**
			 * @brief Accessor for the PublishCompletedEvent.
			 * Invoked when a packet for publish acknowledged, release, received, or complete is received from the broker for a sent publish packet.
			 * Notifies of the general state of the publish process for its corresponding QOS level. Does not call back on QOS 0 publishes.
			 * 
			 * @return Reference to the PublishCompletedEvent instance.
			 */
			PublishCompletedEvent& onPublishCompletedEvent() noexcept;

			/**
			 * @brief Accessor for the SubscribeAckEvent.
			 * Invoked when a subscribe acknowledgment is received from the broker for a sent subscribe packet.
			 * 
			 * @return Reference to the SubscribeAckEvent instance.
			 */
			SubscribeAckEvent& onSubscribeAckEvent() noexcept;

			/**
			 * @brief Accessor for the UnSubscribeAckEvent.
			 * Invoked when an unsubscribe acknowledgment is received from the broker for a sent unsubscribe packet.
			 * 
			 * @return Reference to the UnSubscribeAckEvent instance.
			 */
			UnSubscribeAckEvent& onUnSubscribeAckEvent() noexcept;

			/**
			 * @brief Get the connection status of the MQTT client.
			 * 
			 * @return ConnectionStatus indicating the current connection status of the client.
			 */
			ConnectionStatus getConnectionStatus() const noexcept;

			/**
			 * @brief Get the info of the current MQTT connection.
			 * 
			 * @return MqttConnectionInfo containing details about the current connection.
			 */
			const MqttConnectionInfo& getConnectionInfo() const noexcept;

			/**
			 * @brief Check if the client is configured for asynchronous ticking.
			 * 
			 * @return true if the client is set to tick asynchronously, false otherwise.
			 */
			bool getIsTickAsync() const noexcept;

		private:
			std::unique_ptr<MqttClientImpl> m_impl{ nullptr };
		};
	}
}