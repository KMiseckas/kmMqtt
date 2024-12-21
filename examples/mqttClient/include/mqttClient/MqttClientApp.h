#ifndef INCLUDE_MQTTCLIENT_MQTTCLIENTAPP_H
#define INCLUDE_MQTTCLIENT_MQTTCLIENTAPP_H

#include "imgui.h"
#include <mqttClient/Events/EventService.h>
#include <mqttClient/Views/SessionTabsView.h>
#include <mqttClient/Events/ApplicationEvents.h>

class MqttClientApp 
{
public:
	~MqttClientApp();

	bool init();
	bool drawUi();
	void setWindowSize(const ImVec2& windowSize);
	void exit();
	void reboot();

private:

	static constexpr int k_windowFlags{ ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoTitleBar };

	bool m_requestedExit{ false };
	ImVec2 m_windowSize;
	SessionTabsView* m_sessionTabsView{ nullptr };

};

#endif //INCLUDE_MQTTCLIENT_MQTTCLIENTAPP_H 
