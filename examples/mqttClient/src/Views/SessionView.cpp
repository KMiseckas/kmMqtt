// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <mqttClient/Views/SessionView.h>
#include <mqttClient/ViewHelpers.h>
#include <algorithm>
#include <imgui_internal.h>

void SessionView::draw()
{
	ui::sessionContentStyle();

	//Connection panel
	drawConnectionControlPanel();

	auto topicHeight{ ImGui::GetContentRegionAvail().y * 0.6f };

	//Topics panel
	if (viewHelpers::beginPanel(text::session_subscription_topics_panel_title, { ImGui::GetContentRegionAvail().x * 0.35f, topicHeight }, ImGuiChildFlags_ResizeX))
	{
		drawTopicView();
	}
	viewHelpers::endPanel();

	//Messages sent/received panel
	ImGui::SameLine();
	if (viewHelpers::beginPanel(text::session_messages_panel_title, { ImGui::GetContentRegionAvail().x, topicHeight }))
	{
		drawMessageView();
	}
	viewHelpers::endPanel();

	//Publishing panel
	if (viewHelpers::beginPanel(text::session_publishing_panel_title, { ImGui::GetContentRegionAvail().x * 0.4f ,0 }, ImGuiChildFlags_ResizeX))
	{
		drawPublishView();
	}
	viewHelpers::endPanel();

	//Output log panel
	ImGui::SameLine();
	if (viewHelpers::beginPanel(text::session_log_output_panel_title, { ImGui::GetContentRegionAvail().x,0 }, ImGuiChildFlags_AutoResizeX))
	{
		drawOutputLog();
	}
	viewHelpers::endPanel();

	m_model->tickMqtt();
}

void SessionView::drawConnectionControlPanel()
{
	auto height{ ImGui::GetContentRegionAvail().y * 0.3f};

	if (viewHelpers::beginPanel(text::session_connection_control_panel_title, { 0,0 }, ImGuiChildFlags_AutoResizeY))
	{
		//Configs editing
		//Connection args
		//Connect button
		//Disconnect button
		//Connection state
		//Cancel button

		drawConnectArgs();

		ImGui::NewLine();
		if (m_model->connectArgs.will != nullptr)
		{
			drawWillBtn();
			ImGui::SameLine();
		}
		drawAdvancedOptionsBtn();
		ImGui::SameLine();
		drawConfigs();
		ImGui::SameLine();
		drawConnect();
		ImGui::SameLine();
		drawStatus();
	}
	viewHelpers::endPanel();
}

void SessionView::drawAdvancedOptionsBtn()
{
	const char* btnText = UIData.showingAdvancedConnectOptions ? text::session_connect_hide_advanced_options : text::session_connect_show_advanced_options;

	if (ImGui::Button(btnText, { 0,0 }))
	{
		UIData.showingAdvancedConnectOptions = !UIData.showingAdvancedConnectOptions;
	}
}

void SessionView::drawConfigs()
{
	if (ImGui::Button(BTN_LABEL(text::session_connection_configs_button_label, m_model->getName().c_str()),{}))
	{
		if (ImGui::BeginPopupModal("Popup"))
		{

		}
		ImGui::EndPopup();
	}
}
void SessionView::drawWillBtn()
{
	if (UIData.showingWillOptions)
	{
		if (ImGui::Button(text::session_connect_adv_hide_will_options))
		{
			UIData.showingWillOptions = false;
		}
	}
	else
	{
		if (ImGui::Button(text::session_connect_adv_show_will_options))
		{
			UIData.showingWillOptions = true;
		}
	}
}

void SessionView::drawStatus()
{
	const char* statusTxt{ nullptr };
	ImVec4 color;

	if (m_model->getConnectionStatus() == kmMqtt::mqtt::ConnectionStatus::CONNECTED)
	{
		color = ui::colors::k_green;
		statusTxt = text::session_connection_status_connected;
	}
	else if (m_model->getConnectionStatus() == kmMqtt::mqtt::ConnectionStatus::CONNECTING)
	{
		color = ui::colors::k_green;
		statusTxt = text::session_connection_status_connecting;
	}
	else if (m_model->getConnectionStatus() == kmMqtt::mqtt::ConnectionStatus::DISCONNECTED)
	{
		color = ui::colors::k_red;
		statusTxt = text::session_connection_status_disconnected;
	}
	else if (m_model->getConnectionStatus() == kmMqtt::mqtt::ConnectionStatus::RECONNECTING)
	{
		color = ui::colors::k_yellow;
		statusTxt = text::session_connection_status_reconnecting;
	}
	else
	{
		color = ui::colors::k_yellow;
		statusTxt = "...";
	}

	const auto width = ImGui::CalcTextSize(text::session_connection_status).x + ImGui::CalcTextSize(statusTxt).x + ImGui::GetStyle().ItemSpacing.x;
	const auto xPos = ImGui::GetContentRegionMax().x - width;
	ImGui::SetCursorPosX(xPos);

	ImGui::Text(text::session_connection_status);
	ImGui::SameLine();
	ImGui::TextColored(color, "%s", statusTxt);
}

void SessionView::drawConnect()
{
	if (m_model->getConnectionStatus() == kmMqtt::mqtt::ConnectionStatus::CONNECTED)
	{
		if (ImGui::Button(text::session_connection_disconnect_button_label))
		{
			m_model->disconnect();
		}
	}
	else if (m_model->getConnectionStatus() == kmMqtt::mqtt::ConnectionStatus::CONNECTING)
	{
		if (ImGui::Button(text::session_connection_cancel_button_label))
		{
			m_model->cancel();
		}
	}
	else if (m_model->getConnectionStatus() == kmMqtt::mqtt::ConnectionStatus::DISCONNECTED)
	{
		if (ImGui::Button(text::session_connection_connect_button_label))
		{
			m_model->connect();
		}
	}
	else if (m_model->getConnectionStatus() == kmMqtt::mqtt::ConnectionStatus::RECONNECTING)
	{
		if (ImGui::Button(text::session_connection_cancel_button_label))
		{
			m_model->cancel();
		}
	}
}

void SessionView::drawConnectArgs()
{
	auto height{ ImGui::GetContentRegionAvail().y * 0.8f };

	ui::pushPanelInputFieldStyle();

	drawConnectBasicArgs();

	if (UIData.showingAdvancedConnectOptions)
	{
		drawConnectAdvancedArgs();
	}

	if (UIData.showingWillOptions)
	{
		drawWillArgs();
	}

	ui::popPanelInputFieldStyle();
}

void SessionView::drawConnectBasicArgs()
{
	const float ratioWidth{ ImGui::GetContentRegionAvail().x * 0.33f };
	static constexpr float spacingX{ 16.0f };

	//1ST ROW
	ImGui::Text(text::session_connect_address_input);
	ImGui::SameLine(ratioWidth, spacingX);
	ImGui::Text(text::session_connect_clientid);
	ImGui::SameLine(ratioWidth * 2, spacingX * 2);
	ImGui::Text(text::session_connect_cleanstart);

	ImGui::SetNextItemWidth(ratioWidth);
	if (ImGui::InputTextWithHint("##AddressInput", text::session_connect_address_input_hint, UIData.addressBuffer, sizeof(UIData.addressBuffer)))
	{
		auto address = kmMqtt::mqtt::Address::toAddress(UIData.addressBuffer);

		if (address.size() > 0)
		{
			m_model->connectAddress.primaryAddress = address[0];
		}
	}

	ImGui::SameLine(ratioWidth, spacingX);
	ImGui::SetNextItemWidth(ratioWidth);
	if (ImGui::InputText("##CleanIdInput", UIData.clientIdBuffer, sizeof(UIData.clientIdBuffer)))
	{
		m_model->connectArgs.clientId = UIData.clientIdBuffer;
	}

	ImGui::SameLine(ratioWidth * 2, spacingX * 2);
	if (ImGui::RadioButton("No##cleanStart", UIData.cleanStartSelectedOption == 0))
	{
		UIData.cleanStartSelectedOption = 0;
		m_model->connectArgs.cleanStart = false;
	}

	ImGui::SameLine();
	if (ImGui::RadioButton("Yes##cleanStart", UIData.cleanStartSelectedOption == 1))
	{
		UIData.cleanStartSelectedOption = 1;
		m_model->connectArgs.cleanStart = true;
	}

	///2nd ROW
	ImGui::Text(text::session_connect_username);
	ImGui::SameLine(ratioWidth, spacingX);
	ImGui::Text(text::session_connect_password);
	ImGui::SameLine(ratioWidth * 2, spacingX * 2);
	ImGui::Text(text::session_mqtt_tick_async);

	ImGui::SetNextItemWidth(ratioWidth);
	if (ImGui::InputText("##UsernameInput", UIData.usernameBuffer, sizeof(UIData.usernameBuffer)))
	{
		m_model->connectArgs.username = UIData.usernameBuffer;
	}

	ImGui::SameLine(ratioWidth, spacingX);
	ImGui::SetNextItemWidth(ratioWidth);
	if (ImGui::InputText("##PasswordInput", UIData.passwordBuffer, sizeof(UIData.passwordBuffer), ImGuiInputTextFlags_Password))
	{
		m_model->connectArgs.password = UIData.passwordBuffer;
	}

	ImGui::SameLine(ratioWidth * 2, spacingX * 2);
	if (ImGui::RadioButton("No##TickAsync", UIData.tickAsync == 0))
	{
		UIData.tickAsync = 0;
		m_model->useTickAsync = false;
	}

	ImGui::SameLine();
	if (ImGui::RadioButton("Yes##TickAsync", UIData.tickAsync == 1))
	{
		UIData.tickAsync = 1;
		m_model->useTickAsync = true;
	}
}

void SessionView::drawConnectAdvancedArgs()
{
	ImGui::SeparatorText(text::session_connect_advanced_text_title);

	static constexpr float elementPerc{ 0.25f };
	static constexpr float spacingX{ 16.0f };

	const float ratioWidth{ ImGui::GetContentRegionAvail().x * elementPerc - spacingX };

	//1ST ROW
	ImGui::Text(text::session_connect_adv_keepalivesec);
	ImGui::SameLine(ratioWidth, spacingX);
	ImGui::Text(text::session_connect_adv_session_expiry_interval);
	ImGui::SameLine(ratioWidth * 2, spacingX * 2);
	ImGui::Text(text::session_connect_adv_maximum_receive);
	ImGui::SameLine(ratioWidth * 3, spacingX * 3);
	ImGui::Text(text::session_connect_adv_maximum_topic_aliases);

	ImGui::SetNextItemWidth(ratioWidth);
	if (ImGui::InputInt("##KeepAliveSecInput", &UIData.keepAliveSec))
	{
		m_model->connectArgs.keepAliveInSec = UIData.keepAliveSec;
		UIData.keepAliveSec = m_model->connectArgs.keepAliveInSec;
	}

	ImGui::SameLine(ratioWidth, spacingX);
	ImGui::SetNextItemWidth(ratioWidth);
	if (ImGui::InputInt("##ExpiryIntervalInput", &UIData.expiryInterval))
	{
		m_model->connectArgs.sessionExpiryInterval = UIData.expiryInterval;

		if (UIData.expiryInterval < 0)
		{
			UIData.expiryInterval = std::numeric_limits<std::uint32_t>::max() / 2;
		}
		else
		{
			UIData.expiryInterval = m_model->connectArgs.sessionExpiryInterval;
		}
	}

	ImGui::SameLine(ratioWidth * 2, spacingX * 2);
	ImGui::SetNextItemWidth(ratioWidth);
	if (ImGui::InputInt("##ReceiveMaximumInput", &UIData.receiveMaximum))
	{
		m_model->connectArgs.receiveMaximum = UIData.receiveMaximum;
		UIData.receiveMaximum = m_model->connectArgs.receiveMaximum;
	}

	ImGui::SameLine(ratioWidth * 3, spacingX * 3);
	ImGui::SetNextItemWidth(ratioWidth);
	if (ImGui::InputInt("##MaxTopicAliases", &UIData.maxTopicAliases))
	{
		m_model->connectArgs.maximumTopicAliases = UIData.maxTopicAliases;
		UIData.maxTopicAliases = m_model->connectArgs.maximumTopicAliases;
	}

	//2nd ROW
	ImGui::Text(text::session_connect_adv_request_response_info);
	ImGui::SameLine(ratioWidth, spacingX);
	ImGui::Text(text::session_connect_adv_request_problem_info);
	ImGui::SameLine(ratioWidth * 2, spacingX * 2);
	ImGui::Text(text::session_connect_adv_extended_auth_method);
	ImGui::SameLine(ratioWidth * 3, spacingX * 3);
	ImGui::Text(text::session_connect_adv_extended_auth_data);

	if (ImGui::RadioButton("No##responseInfo", m_model->connectArgs.requestResponseInformation == false))
	{
		m_model->connectArgs.requestResponseInformation = false;
	}

	ImGui::SameLine();
	if (ImGui::RadioButton("Yes##responseInfo", m_model->connectArgs.requestResponseInformation == true))
	{
		m_model->connectArgs.requestResponseInformation = true;
	}

	ImGui::SameLine(ratioWidth, spacingX);
	if (ImGui::RadioButton("No##problemInfo", m_model->connectArgs.requestProblemInformation == false))
	{
		m_model->connectArgs.requestProblemInformation = false;
	}

	ImGui::SameLine();
	if (ImGui::RadioButton("Yes##problemInfo", m_model->connectArgs.requestProblemInformation == true))
	{
		m_model->connectArgs.requestProblemInformation = true;
	}

	ImGui::SetNextItemWidth(ratioWidth);
	ImGui::SameLine(ratioWidth * 2, spacingX * 2);
	if (ImGui::InputText("##ExtendedAuthMethod", UIData.extendedAuthMethod, sizeof(UIData.extendedAuthMethod)))
	{
		m_model->connectArgs.extendedAuthenticationMethod = UIData.extendedAuthMethod;
	}

	ImGui::SameLine(ratioWidth * 3, spacingX * 3);
	ImGui::SetNextItemWidth(ratioWidth);
	if (ImGui::InputText("##ExtendedAuthData", UIData.extendedAuthData, sizeof(UIData.extendedAuthData)))
	{
		m_model->connectArgs.extendedAuthenticationData = std::move(
			std::make_unique<kmMqtt::mqtt::BinaryData>(
				kmMqtt::mqtt::BinaryData(
					static_cast<std::uint16_t>(std::strlen(UIData.extendedAuthData)),
					(const uint8_t*)UIData.extendedAuthData)));
	}

	//3rd ROW
	ImGui::Text(text::session_connect_adv_will);

	if (ImGui::RadioButton("No##will", m_model->connectArgs.will == nullptr))
	{
		m_model->connectArgs.will = nullptr;
		UIData.showingWillOptions = false;
	}

	ImGui::SameLine();
	if (ImGui::RadioButton("Yes##will", m_model->connectArgs.will != nullptr))
	{
		m_model->connectArgs.will = std::make_unique<kmMqtt::mqtt::Will>("");
		UIData.showingWillOptions = true;
	}
}

void SessionView::drawWillArgs()
{
	ImGui::SeparatorText(text::session_connect_will_options_text_title);

	static constexpr float elementPerc{ 0.25f };
	static constexpr float spacingX{ 16.0f };

	const float ratioWidth{ ImGui::GetContentRegionAvail().x * elementPerc - spacingX };

	//1ST ROW
	ImGui::Text(text::session_connect_will_QOS);
	ImGui::SameLine(ratioWidth, spacingX);
	ImGui::Text(text::session_connect_will_retain_will_msg);
	ImGui::SameLine(ratioWidth * 2, spacingX * 2);
	ImGui::Text(text::session_connect_will_delay_interval);
	ImGui::SameLine(ratioWidth * 3, spacingX * 3);
	ImGui::Text(text::session_connect_will_msg_expiry_interval);

	if (ImGui::RadioButton("0##WillQos", m_model->connectArgs.will->willQos == kmMqtt::mqtt::Qos::QOS_0))
	{
		m_model->connectArgs.will->willQos = kmMqtt::mqtt::Qos::QOS_0;
	}

	ImGui::SameLine();
	if (ImGui::RadioButton("1##WillQos", m_model->connectArgs.will->willQos == kmMqtt::mqtt::Qos::QOS_1))
	{
		m_model->connectArgs.will->willQos = kmMqtt::mqtt::Qos::QOS_1;
	}

	ImGui::SameLine();
	if (ImGui::RadioButton("2##WillQos", m_model->connectArgs.will->willQos == kmMqtt::mqtt::Qos::QOS_2))
	{
		m_model->connectArgs.will->willQos = kmMqtt::mqtt::Qos::QOS_2;
	}

	ImGui::SameLine(ratioWidth, spacingX);
	if (ImGui::RadioButton("No##WillRetainMsg", m_model->connectArgs.will->retainWillMessage == false))
	{
		m_model->connectArgs.will->retainWillMessage = false;
	}

	ImGui::SameLine();
	if (ImGui::RadioButton("Yes##WillRetainMsg", m_model->connectArgs.will->retainWillMessage == true))
	{
		m_model->connectArgs.will->retainWillMessage = true;
	}

	ImGui::SameLine(ratioWidth * 2, spacingX * 2);
	ImGui::SetNextItemWidth(ratioWidth);
	if (ImGui::InputInt("##WillDelayInterval", &UIData.willDelayInterval))
	{
		m_model->connectArgs.will->willDelayInterval = UIData.willDelayInterval;

		if (UIData.willDelayInterval < 0)
		{
			UIData.willDelayInterval = std::numeric_limits<std::uint32_t>::max() / 2;
		}
		else
		{
			UIData.willDelayInterval = m_model->connectArgs.will->willDelayInterval;
		}
	}

	ImGui::SameLine(ratioWidth * 3, spacingX * 3);
	ImGui::SetNextItemWidth(ratioWidth);
	if (ImGui::InputInt("##WillMsgExpiryInterval", &UIData.willMsgExpiryInterval))
	{
		m_model->connectArgs.will->messageExpiryInterval = UIData.willMsgExpiryInterval;

		if (UIData.willMsgExpiryInterval < 0)
		{
			UIData.willMsgExpiryInterval = std::numeric_limits<std::uint32_t>::max() / 2;
		}
		else
		{
			UIData.willMsgExpiryInterval = m_model->connectArgs.will->messageExpiryInterval;
		}
	}

	//2nd ROW
	ImGui::Text(text::session_connect_will_content_type);
	ImGui::SameLine(ratioWidth, spacingX);
	ImGui::Text(text::session_connect_will_payload_format);
	ImGui::SameLine(ratioWidth * 2, spacingX * 2);
	ImGui::Text(text::session_connect_will_topic);
	ImGui::SameLine(ratioWidth * 3, spacingX * 3);
	ImGui::Text(text::session_connect_will_payload);

	ImGui::SetNextItemWidth(ratioWidth);
	if (ImGui::InputText("##WillContentType", UIData.willContentType, sizeof(UIData.willContentType)))
	{
		m_model->connectArgs.will->contentType = UIData.willContentType;
	}

	ImGui::SameLine(ratioWidth, spacingX);
	if (ImGui::RadioButton("UTF8##WillPldFormatType", m_model->connectArgs.will->payloadFormat == kmMqtt::mqtt::PayloadFormatIndicator::UTF8))
	{
		m_model->connectArgs.will->payloadFormat = kmMqtt::mqtt::PayloadFormatIndicator::UTF8;
	}

	ImGui::SameLine();
	if (ImGui::RadioButton("Binary##WillPldFormatType", m_model->connectArgs.will->payloadFormat == kmMqtt::mqtt::PayloadFormatIndicator::BINARY))
	{
		m_model->connectArgs.will->payloadFormat = kmMqtt::mqtt::PayloadFormatIndicator::BINARY;
	}

	ImGui::SameLine(ratioWidth * 2, spacingX * 2);
	ImGui::SetNextItemWidth(ratioWidth);
	if (ImGui::InputText("##WillTopic", UIData.willTopic, sizeof(UIData.willTopic)))
	{
		m_model->connectArgs.will->willTopic = UIData.willTopic;
	}

	ImGui::SameLine(ratioWidth * 3, spacingX * 3);
	ImGui::SetNextItemWidth(ratioWidth);
	static bool showPayloadEditor{ false };
	if (ImGui::Button("Edit Payload"))
	{
		ImGui::OpenPopup("Will Payload Editor##WillPayloadEditor");
		showPayloadEditor = true;
	}

	if (showPayloadEditor)
	{
		if (ImGui::BeginPopupModal("Will Payload Editor##WillPayloadEditor", &showPayloadEditor, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::InputTextMultiline("##WillTopic", UIData.willPayload, sizeof(UIData.willPayload));
			ImGui::EndPopup();
		}

		if (!showPayloadEditor)
		{
			m_model->connectArgs.will->payload = std::make_unique<kmMqtt::mqtt::BinaryData>(kmMqtt::mqtt::BinaryData(std::strlen(UIData.willPayload), reinterpret_cast<const uint8_t*>(UIData.willPayload)));
		}
	}

	//3rd ROW
	ImGui::Text(text::session_connect_will_response_topic);

	ImGui::SetNextItemWidth(ratioWidth);
	if (ImGui::InputText("##WillResponseTopic", UIData.willResponseTopic, sizeof(UIData.willResponseTopic)))
	{
		m_model->connectArgs.will->responseTopic = UIData.willResponseTopic;
	}
}

void SessionView::drawTopicView()
{
	if (getModel()->isMqttConnected)
	{
		m_topicView.draw();
	}
	else
	{
		ImGui::TextWrapped(text::session_subscription_topics_panel_not_connected_msg);
	}
}

void SessionView::drawMessageView()
{
	if (getModel()->isMqttConnected)
	{
		m_messageView.draw();
	}
	else
	{
		ImGui::TextWrapped(text::session_messages_panel_not_connected_msg);
	}
}

void SessionView::drawPublishView()
{
	if (getModel()->isMqttConnected)
	{
		m_publishView.draw();
	}
	else
	{
		ImGui::TextWrapped("Connect to MQTT broker to publish messages");
	}
}

void SessionView::drawOutputLog()
{
	m_outputView.draw();
}

void SessionView::handleAppliedModel(ModelPtr oldModel, ModelPtr newModel)
{
	m_topicView.setModel(newModel->topicModel);
	m_messageView.setModel(newModel->messagesModel);
	m_publishView.setModel(newModel->publishModel);
	m_outputView.setModel(newModel->outputModel);
}
