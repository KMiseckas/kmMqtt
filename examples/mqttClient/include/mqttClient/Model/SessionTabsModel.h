// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_MQTTCLIENT_MODEL_SESSIONTABSMODEL_H
#define INCLUDE_MQTTCLIENT_MODEL_SESSIONTABSMODEL_H

#include "mqttClient/Model/ViewModel.h"
#include "mqttClient/Model/SessionModel.h"
#include "mqttClient/Events/DirectEvent.h"

#include <vector>
#include <functional>

using SessionModelPtr = std::shared_ptr<SessionModel>;

using SessionAddedCallback = std::function<void(SessionModelPtr)>;
using SessionRemovedCallback = std::function<void(std::uint8_t)>;

using OpenSessionChangedEvent = events::DirectEvent<SessionModelPtr>;

class SessionTabsModel : public ViewModel
{
public:
	~SessionTabsModel() override;

	bool addSession(SessionAddedCallback callback = nullptr);
	void removeSession(std::uint8_t index, SessionRemovedCallback callback = nullptr);
	void setOpenSession(std::uint8_t index) noexcept;

	std::uint8_t getNumberOfSession() noexcept;
	std::uint8_t getOpenSessionIndex() noexcept;

	SessionModelPtr getSessionModel(std::uint8_t index) noexcept;

	static constexpr std::uint8_t k_maxSessionsAllowed{ 8U };

	OpenSessionChangedEvent onOpenSessionChangedEvent;
private:
	std::vector<SessionModelPtr> m_sessions;
	std::uint8_t m_openSessionIndex{ 0 };
};

#endif //INCLUDE_MQTTCLIENT_MODEL_SESSIONTABSMODEL_H 
