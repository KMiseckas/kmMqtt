// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_ISESSIONSTATEPERSISTANTSTORE_H
#define INCLUDE_KMMQTT_MQTT_ISESSIONSTATEPERSISTANTSTORE_H

#include "kmMqtt/GlobalMacros.h"
#include "kmMqtt/Mqtt/State/SessionState/MessageContainer.h"
#include <vector>
#include <string>
#include <cstdint>

namespace kmMqtt 
{
    namespace mqtt 
    {
        /**
         * @brief Interface for storing and retrieving MQTT session state.
         *
         * This interface allows SDK users to implement custom persistence mechanisms
         * for MQTT session state, specifically for QoS 1 and QoS 2 publish messages that need to be tracked across sessions.
         */
        class PUBLIC_API ISessionStatePersistantStore
        {
        public:
            ISessionStatePersistantStore() = default;
            virtual ~ISessionStatePersistantStore() = default;

            /**
             * @brief Initializes the session state store for a specific client ID.
             *
             * This method is called when an MQTT client finishes connecting.
             *
             * @param clientId The identifier of the mqtt client.
             * @return true if the initialization was successful, false otherwise.
			 */
            virtual bool initialize(const char* clientId) = 0;

            /**
             * @brief Writes a Publish packets to the session state persistant storage.
             *
             * This method is called when an outgoing QoS 1 or QoS 2
             * PUBLISH packet is sent, or when an incoming QoS 2 PUBLISH packet
             * is received but not yet fully acknowledged.
             *
             * @param clientId The identifier of the mqtt client.
             * @param sessionExpiryInterval The session expiry interval after which session is considered obsolete.
             * @param msgData The Publish packet to be stored.
             * @return true if the packet was successfully written to the session state storage, false otherwise.
             */
            virtual bool write(const char* clientId, uint32_t sessionExpiryInterval, const SavedData& data) = 0;

            /**
             * @brief Reads all persisted Publish packets from the session state store. These NEED to be read in the same
             * order as they have been written. I.E. Last message written needs to be the last message in the `outMsgData` vector.
             *
             * This method is called when an MQTT client attempts to resume a valid
             * session (after broker connect response).
             *
             * @param clientId The identifier of the mqtt client.
             * @param[out] packets A vector that will be populated with the Publish packets
             * read from the session state.
             * @return true if the session state was successfully read (even if no packets
             * were stored), false if an error occurred during reading.
             */
            virtual bool readAll(const char* clientId, std::vector<const SavedData>& outData) = 0;

            /**
             * @brief Removes a specific Publish packets from the session state store.
             *
             * This method will be called after a QoS 1 message is acknowledged,
             * a QoS 2 message is fully acknowledged,
             * or when a message is deemed no longer necessary to keep in the session state.
             *
             * @param clientId The identifier of the mqtt client.
             * @param packetId The identifier of the packet to remove.
             * @return true if the packet was successfully removed or was not found,
             * false if an error occurred during removal.
             */
            virtual bool removeMessage(const char* clientId, std::uint16_t packetId) = 0;

            /**
             * @brief Updates a specific Publish packet in the session state store.
             *
             * @param clientId The identifier of the mqtt client.
             * @param packetId The identifier of the packet to update.
			 * @param newStatus The new status of the packet.
			 * @param bringToFront If true, the packet will be moved to the end of the list.
             * @return true if the packet was successfully updated, false otherwise.
			 */
			virtual bool updateMessage(const char* clientId, std::uint16_t packetId, PublishMessageStatus newStatus, bool bringToEnd) = 0;

            /**
             * @brief Remove all data associated with this client id from store.
             *
             * This might be called when a clean session is started, or when the
             * user explicitly requests to clear any stored session data.
             *
             * @param clientId The identifier of the mqtt client.
             * @return true if the session state was successfully cleared or not found, false otherwise.
             */
            virtual bool removeFromStore(const char* clientId) = 0;

			/**
             * @brief Remove all expired session states in store.
             *
             * Called at MQTT start-up before connection.
             * 
             * @return true if expired session have been removed or none to remove were found.
             */
			virtual bool removeExpiredFromStore() = 0;
		};

    }
}

#endif //INCLUDE_KMMQTT_MQTT_ISESSIONSTATEPERSISTANTSTORE_H
