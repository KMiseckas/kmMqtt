#ifndef INCLUDE_MQTTCLIENT_VIEWS_SESSIONTABSVIEW_H
#define INCLUDE_MQTTCLIENT_VIEWS_SESSIONTABSVIEW_H

#include "mqttClient/Views/View.h"
#include <mqttClient/Model/SessionTabsModel.h>
#include <mqttClient/Views/SessionView.h>
#include <vector>

class SessionTabsView : public View<SessionTabsModel>
{
public:
	SessionTabsView() noexcept
	{
	}

	~SessionTabsView() override
	{
	}

	void draw() override;

protected:

	void drawDefaultView();
	void drawSessionsView();

	void handleAppliedModel(ModelPtr oldModel, ModelPtr newModel) override;

	SessionView m_activeSessionView;
	events::ListenerHandle m_onSessionChangedEventHandle{ 0 };
};

#endif //INCLUDE_MQTTCLIENT_VIEWS_SESSIONTABSVIEW_H 
