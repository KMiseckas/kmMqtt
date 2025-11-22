#ifndef MQTTCLIENT_VIEWS_OUTPUTVIEW_H
#define MQTTCLIENT_VIEWS_OUTPUTVIEW_H

#include <mqttClient/Views/View.h>
#include <mqttClient/Model/OutputModel.h>
#include <string>

class OutputView : public View<OutputModel>
{
public:
	OutputView() noexcept
	{
	}

	~OutputView() override
	{
	}

	void draw() override;

protected:
	void drawLogOutput();
	void drawClearConfirmation();
	void copyAllLogsToClipboard();
	ImVec4 getLogLevelColor(cleanMqtt::LogLevel logLevel) const;

	void handleAppliedModel(ModelPtr oldModel, ModelPtr newModel) override;
};

#endif // MQTTCLIENT_VIEWS_OUTPUTVIEW_H