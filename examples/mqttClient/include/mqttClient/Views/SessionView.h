// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_MQTTCLIENT_VIEWS_SESSIONVIEW_H
#define INCLUDE_MQTTCLIENT_VIEWS_SESSIONVIEW_H

#include <mqttClient/Views/View.h>
#include <mqttClient/Model/SessionModel.h>
#include "TopicsView.h"
#include "PublishView.h"
#include "MessagesView.h"
#include "OutputView.h"

#define UIData m_model->uiData

class SessionView : public View<SessionModel>
{
public:
	SessionView() noexcept
	{
	}

	~SessionView() override
	{
	}

	void draw() override;

protected:

	void drawConnectionControlPanel();
	void drawAdvancedOptionsBtn();
	void drawConfigs();
	void drawWillBtn();
	void drawConnect();
	void drawStatus();
	void drawConnectArgs();
	void drawConnectBasicArgs();
	void drawConnectAdvancedArgs();
	void drawWillArgs();
	void drawTopicView();
	void drawPublishView();
	void drawOutputLog();
	void drawMessageView();

	void handleAppliedModel(ModelPtr oldModel, ModelPtr newModel) override;

	TopicsView m_topicView;
	MessagesView m_messageView;
	PublishView m_publishView;
	OutputView m_outputView;
};

#endif //INCLUDE_MQTTCLIENT_VIEWS_SESSIONVIEW_H