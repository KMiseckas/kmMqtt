#include <kmMqtt/Mqtt/State/SessionState/SessionState.h>
#include <kmMqtt/Mqtt/Enums/ClientErrorCode.h>

namespace kmMqtt
{
	namespace mqtt
	{
		SessionState::SessionState(const char* clientId,
			std::uint32_t sessionExpiryInterval,
			std::uint32_t retryInterval) noexcept :
			m_clientId{ clientId },
			m_retryInterval{ Milliseconds(retryInterval)},
			m_persistantStore{ nullptr }, //TODO: Add persistant store parameter later.
			m_sessionExpiryInterval{ Milliseconds(sessionExpiryInterval)}
		{
		}

		SessionState::SessionState(const SessionState& other) noexcept
		{
			if(this != &other)
			{
				LockGuard guardThis{ m_mutex };
				LockGuard guardOther{ other.m_mutex };
				m_clientId = other.m_clientId;
				m_retryInterval = other.m_retryInterval;
				m_persistantStore = other.m_persistantStore;
				m_sessionExpiryInterval = other.m_sessionExpiryInterval;
				m_messages = other.m_messages;
			}
		}

		SessionState& SessionState::operator=(const SessionState& other) noexcept
		{
			if (this != &other)
			{
				LockGuard guardThis{ m_mutex };
				LockGuard guardOther{ other.m_mutex };
				m_clientId = other.m_clientId;
				m_retryInterval = other.m_retryInterval;
				m_persistantStore = other.m_persistantStore;
				m_sessionExpiryInterval = other.m_sessionExpiryInterval;
				m_messages = other.m_messages;
			}
			return *this;
		}

		ClientErrorCode SessionState::addPrevSessionState(const SessionState& prevSessionState) noexcept
		{
			for (const auto& msgData : prevSessionState.m_messages)
			{
				if (!m_messages.contains(msgData.data.packetID))
				{
					ClientErrorCode err{ addPrevStateMessage(msgData.data.packetID, msgData.data.publishMsgData) };

					if (err != ClientErrorCode::No_Error)
					{
						return err;
					}
				}
			}

			return { ClientErrorCode::No_Error };
		}

		ClientErrorCode SessionState::addMessage(const std::uint16_t packetId, PublishMessageData publishMsgData) noexcept
		{
			TimePoint nextRetryTime{ std::chrono::steady_clock::now() + m_retryInterval };
			MessageContainerData data{ packetId, std::move(publishMsgData), std::move(nextRetryTime), m_retryInterval.count() > 0};

			{
				LockGuard guard{ m_mutex };

				//TODO: Persistant Storage - needs rethink to work async. Commented out until future implementation.
				/*if (m_persistantStore != nullptr)
				{
					if (!m_persistantStore->write(m_clientId, static_cast<std::uint32_t>(m_sessionExpiryInterval.count()), data.data))
					{
						LogError("SessionState", "Failed to write to session state message to persistant storage, Client ID: %s, Packet ID: %d", m_clientId, packetId);
					}

					LogTrace("SessionState", "Session state message written to persistant storage, Client ID: %s, Packet ID: %d", m_clientId, packetId);
				}*/

				m_messages.push(std::move(data));
			}

			return ClientErrorCode::No_Error;
		}

		ClientErrorCode SessionState::addPrevStateMessage(const std::uint16_t packetId, const PublishMessageData& publishMsgData) noexcept
		{
			//std::chrono::steady_clock::now() to Retry ASAP when restoring previous session state messages.
			MessageContainerData data{ packetId, std::move(publishMsgData), std::chrono::steady_clock::now(), true };

			//TODO: Persistant Storage - needs rethink to work async. Commented out until future implementation.
			/*if (m_persistantStore != nullptr)
			{
				if (!m_persistantStore->write(m_clientId, static_cast<std::uint32_t>(m_sessionExpiryInterval.count()), data.data))
				{
					LogWarning("SessionState", "Failed to write to previous session state message to persistant storage, Client ID: %s, Packet ID: %d", m_clientId, packetId);
				}

				LogTrace("SessionState", "Previous session state message written to persistant storage, Client ID: %s, Packet ID: %d", m_clientId, packetId);
			}*/

			m_messages.push(std::move(data));

			return ClientErrorCode::No_Error;
		}

		void SessionState::updateMessage(const std::uint16_t packetId, PublishMessageStatus newStatus) noexcept
		{
			{
				LockGuard guard{ m_mutex };

				auto iter{ *(m_messages.get(packetId)) };
				if (iter == m_messages.end())
				{
					LogWarning("SessionState", "Failed to update message, Packet ID: %d not found in session state", packetId);
					return;
				}

				iter->nextRetryTime = std::chrono::steady_clock::now() + m_retryInterval;

				if (newStatus == iter->data.status)
				{
					return;
				}

				iter->data.status = newStatus;

				const bool bringToFront{ shouldBringToFront(iter->data.status, newStatus) };
				if (bringToFront)
				{
					m_messages.moveToEnd(packetId);
				}

				//TODO: Persistant Storage - needs rethink to work async. Commented out until future implementation.
				//m_persistantStore->updateMessage(m_clientId, packetId, newStatus, bringToFront);
			}
		}
		
		void SessionState::removeMessage(const std::uint16_t packetId) noexcept
		{
			{
				LockGuard guard{ m_mutex };

				m_messages.erase(packetId);

				//TODO: Persistant Storage - needs rethink to work async. Commented out until future implementation.
				/*if (m_persistantStore != nullptr)
				{
					if (!m_persistantStore->removeMessage(m_clientId, packetId))
					{
						LogWarning("SessionState", "Failed to remove session state message from persistant storage, Client ID: %s, Packet ID: %d", m_clientId, packetId);
					}

					LogTrace("SessionState", "Session state message removed from persistant storage, Client ID: %s, Packet ID: %d", m_clientId, packetId);
				}*/
			}
		}

		void SessionState::clear() noexcept
		{
			{
				LockGuard guard{ m_mutex };

				m_messages.clear();

				//TODO: Persistant Storage - needs rethink to work async. Commented out until future implementation.
				/*if (m_persistantStore != nullptr)
				{
					if (!m_persistantStore->removeFromStore(m_clientId))
					{
						LogWarning("SessionState", "Failed to clear session state messages from persistant storage, Client ID: %s", m_clientId);
					}

					LogTrace("SessionState", "Session state messages cleared from persistant storage, Client ID: %s", m_clientId);
				}*/
			}
		}

		bool SessionState::shouldBringToFront(PublishMessageStatus oldStatus, PublishMessageStatus newStatus) const noexcept
		{
			if(oldStatus == newStatus)
			{
				return false;
			}

			switch (newStatus)
			{
			case PublishMessageStatus::WaitingForPubComp:
			case PublishMessageStatus::WaitingForPubRel:
				return true;
			default:
				return false;
			}
		}
	}
}
