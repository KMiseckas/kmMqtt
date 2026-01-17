// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include "mqttClient/MqttClientApp.h"
#include <cassert>
#include <mqttClient/Styles.h>

MqttClientApp::~MqttClientApp()
{
	delete m_sessionTabsView;
}

bool MqttClientApp::init()
{
	m_sessionTabsView = new SessionTabsView();
	auto model = std::make_shared<SessionTabsModel>();
	m_sessionTabsView->setModel(model);

	//Events subscriptions
	events::subscribe<events::ExitAppRequestEvent>([&](const events::ExitAppRequestEvent&) { exit(); });
	events::subscribe<events::RebootRequestEvent>([&](const events::RebootRequestEvent&) { reboot(); });
	
	return true;
}

bool MqttClientApp::drawUi()
{
	ImGui::SetNextWindowPos({ 0,0 });
	ImGui::SetNextWindowSize(m_windowSize);
 
	ui::FirstWindowStyle();
	if (ImGui::Begin("Mqtt Client", nullptr, k_windowFlags))
	{

		assert(m_sessionTabsView != nullptr);

		m_sessionTabsView->draw();

		ImGui::End();
	}

	return !m_requestedExit;
}

void MqttClientApp::setWindowSize(const ImVec2& windowSize)
{
	m_windowSize = windowSize;
}

void MqttClientApp::exit()
{
	m_requestedExit = true;
}

void MqttClientApp::reboot()
{
	//TODO full reset
}