#ifndef INCLUDE_MQTTCLIENT_VIEWS_SESSIONVIEW_H
#define INCLUDE_MQTTCLIENT_VIEWS_SESSIONVIEW_H

#include <mqttClient/Views/View.h>
#include <mqttClient/Model/SessionModel.h>

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

	void handleAppliedModel(ModelPtr oldModel, ModelPtr newModel) override;

	const ImVec4 k_red{ 1, 0, 0, 1 };
	const ImVec4 k_green{ 0, 1, 0, 1 };
	const ImVec4 k_yellow{ 0, 1, 1, 1 };
};

#endif //INCLUDE_MQTTCLIENT_VIEWS_SESSIONVIEW_H 
