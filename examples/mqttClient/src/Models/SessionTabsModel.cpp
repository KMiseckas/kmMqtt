#include <mqttClient/Model/SessionTabsModel.h>
#include <cassert>

SessionTabsModel::~SessionTabsModel()
{
	m_openSessionIndex = 0;
	m_sessions.clear();
}

bool SessionTabsModel::addSession(SessionAddedCallback callback)
{
	if (m_sessions.size() >= k_maxSessionsAllowed)
	{
		return false;
	}

	SessionModelPtr model = std::make_shared<SessionModel>();
	model->setIndex(static_cast<std::uint8_t>(m_sessions.size()));
	model->setName("New Session");

	m_sessions.push_back(std::move(model));
	setOpenSession(static_cast<std::uint8_t>(m_sessions.size() - 1));

	if (callback != nullptr)
	{
		callback(m_sessions.at(m_openSessionIndex));
	}

	return true;
}

void SessionTabsModel::removeSession(std::uint8_t index, SessionRemovedCallback callback)
{
	if (m_sessions.size() == 0) return;

	assert(index < m_sessions.size());

	if (m_openSessionIndex == index)
	{
		if (index != 0)
		{
			setOpenSession(m_openSessionIndex - 1);
		}
	}

	m_sessions.erase(m_sessions.begin() + index);

	if (callback != nullptr)
	{
		callback(index);
	}
}

void SessionTabsModel::setOpenSession(std::uint8_t index) noexcept
{
	m_openSessionIndex = index;
	onOpenSessionChangedEvent.invoke(getSessionModel(index));
}

std::uint8_t SessionTabsModel::getOpenSessionIndex() noexcept
{
	return m_openSessionIndex;
}

std::uint8_t SessionTabsModel::getNumberOfSession() noexcept
{
	return static_cast<std::uint8_t>(m_sessions.size());
}

SessionModelPtr SessionTabsModel::getSessionModel(std::uint8_t index) noexcept
{
	assert(index < m_sessions.size());

	return m_sessions[index];
}
