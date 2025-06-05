#include <cleanMqtt/Mqtt/SessionState/SessionState.h>
#include <cleanMqtt/Mqtt/Enums/ClientErrorCode.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		SessionState::SessionState(const char* clientId,
			std::uint32_t sessionExpiryInterval,
			std::uint32_t retryInterval,
			ISessionStatePersistantStore* persistantStore) :
			m_clientId{ clientId },
			m_retryInterval{ Milliseconds(retryInterval)},
			m_persistantStore { persistantStore },
			m_sessionExpiryInterval{ Milliseconds(sessionExpiryInterval)}
		{
		}

		ClientErrorCode SessionState::addMessage(const PacketID packetId, PublishMessageData publishMsgData) noexcept
		{
			TimePoint nextRetryTime{ std::chrono::steady_clock::now() + m_retryInterval };
			MessageContainerData data{ packetId, std::move(publishMsgData), std::move(nextRetryTime), m_retryInterval.count() > 0};

			if (m_persistantStore != nullptr)
			{
				if (!m_persistantStore->write(m_clientId, static_cast<std::uint32_t>(m_sessionExpiryInterval.count()), data.data))
				{
					LogError("SessionState", "Failed to write to session state message to persistant storage, Client ID: %s, Packet ID: %d", m_clientId, packetId);
					return ClientErrorCode::Failed_Writing_To_Persistent_Storage;
				}

				LogTrace("SessionState", "Session state message written to persistant storage, Client ID: %s, Packet ID: %d", m_clientId, packetId);
			}

			const auto iter{ m_messages.push(std::move(data)) };

			return ClientErrorCode::No_Error;
		}

		void SessionState::updateMessage(const PacketID packetId, PublishMessageStatus newStatus) noexcept
		{
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
			if(bringToFront)
			{
				m_messages.moveToEnd(packetId);
			}

			m_persistantStore->updateMessage(m_clientId, packetId, newStatus, bringToFront);
		}
		
		void SessionState::removeMessage(const PacketID packetId) noexcept
		{
			m_messages.erase(packetId);

			if (m_persistantStore != nullptr)
			{
				if (!m_persistantStore->removeMessage(m_clientId, packetId))
				{
					LogWarning("SessionState", "Failed to remove session state message from persistant storage, Client ID: %s, Packet ID: %d", m_clientId, packetId);
				}

				LogTrace("SessionState", "Session state message removed from persistant storage, Client ID: %s, Packet ID: %d", m_clientId, packetId);
			}
		}

		void SessionState::clear() noexcept
		{
			m_messages.clear();

			if (m_persistantStore != nullptr)
			{
				if (!m_persistantStore->removeFromStore(m_clientId))
				{
					LogWarning("SessionState", "Failed to clear session state messages from persistant storage, Client ID: %s", m_clientId);
				}

				LogTrace("SessionState", "Session state messages cleared from persistant storage, Client ID: %s", m_clientId);
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
