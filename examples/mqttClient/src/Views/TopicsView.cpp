#include <mqttClient/Views/TopicsView.h>
#include <mqttClient/Model/TopicsModel.h>
#include <imgui.h>
#include <string>

void TopicsView::draw()
{
	assert(getModel() != nullptr);

	drawTopicsList();
	drawUnsubscribeConfirmation();
}

void TopicsView::drawTopicsList()
{
	drawAddTopicSection();

	ImGui::Separator();

	const auto& topics{ m_model->getSubscribedTopics() };

	if (topics.empty())
	{
		ImGui::TextColored(ui::colors::k_grey, text::topics_none_subscribed_status);
		return;
	}

	//Render each topic as a one-liner with expand functionality
	for (size_t i = 0; i < topics.size(); ++i)
	{
		drawTopicItem(i, topics[i]);
	}
}

void TopicsView::drawAddTopicSection()
{
	if (!m_model->uiData.addingNewTopic)
	{
		//Show + button to add new topic
		if (ImGui::Button(BTN_LABEL(text::topics_subscribe_text, "addTopic"), ImVec2(25, 25)))
		{
			memset(m_model->uiData.newTopicBuffer, 0, sizeof(m_model->uiData.newTopicBuffer));

			m_model->uiData.addingNewTopic = true;
			m_model->uiData.selectedTopicQos = 0;
			m_model->uiData.retainHandling = false;
			m_model->uiData.noLocal = false;
			m_model->uiData.retainAsPublished = false;
		}

		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip(text::topics_subscribe_add_topic_tooltip);
		}
	}
	else
	{
		// Get the available width for layout
		float availableWidth = ImGui::GetContentRegionAvail().x;
		float spacing = ImGui::GetStyle().ItemSpacing.x;

		// Calculate required widths
		const auto cancelBtnSize = ImGui::CalcTextSize(text::topics_subscribe_cancel_add_topic);
		const auto subscribeBtnSize = ImGui::CalcTextSize(text::topics_subscribe_add_topic);
		float totalButtonWidth = cancelBtnSize.x + subscribeBtnSize.x + (spacing * 6);

		ImGui::SetNextItemWidth(-totalButtonWidth);
		bool enterPressed = ImGui::InputTextWithHint("##NewTopicInput", "Enter topic filter...",
			m_model->uiData.newTopicBuffer, sizeof(m_model->uiData.newTopicBuffer),
			ImGuiInputTextFlags_EnterReturnsTrue);

		ImGui::SameLine();

		//Subscribe button
		bool canSubscribe = strlen(m_model->uiData.newTopicBuffer) > 0;
		if (!canSubscribe)
		{
			ImGui::BeginDisabled();
		}

		bool subscribeClicked = ImGui::Button(BTN_LABEL(text::topics_subscribe_add_topic, "subscribe"));

		if (!canSubscribe)
		{
			ImGui::EndDisabled();
		}

		if (ImGui::IsItemHovered() && canSubscribe)
		{
			ImGui::SetTooltip(text::topics_subscribe_add_topic_tooltip);
		}

		ImGui::SameLine();

		//Cancel button
		if (ImGui::Button(BTN_LABEL(text::topics_subscribe_cancel_add_topic, "cancel")))
		{
			m_model->uiData.addingNewTopic = false;
		}

		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip(text::topics_subscribe_cancel_add_topic_tooltip);
		}

		//Handle subscription
		if ((subscribeClicked || enterPressed) && canSubscribe)
		{
			std::string topicFilter(m_model->uiData.newTopicBuffer);
			m_model->subscribe(topicFilter);
			m_model->uiData.addingNewTopic = false;
		}

		//Show advanced options if adding
		if (ImGui::CollapsingHeader(BTN_LABEL(text::topics_advanced_options, "advOptions")))
		{
			drawSubscribeOptions();
		}
	}
}

void TopicsView::drawTopicItem(size_t index, const SubscribedTopic& topic)
{
	ImGui::PushID(static_cast<int>(index));

	const bool isExpanded{ (m_model->uiData.expandedTopicIndex == static_cast<int>(index)) };

	ImGui::AlignTextToFramePadding();

	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0, 0, 0, 0));

	//Expand/collapse arrow
	bool nodeOpen = ImGui::TreeNodeEx("##TopicNode",
		ImGuiTreeNodeFlags_Framed |
		ImGuiTreeNodeFlags_SpanFullWidth |
		ImGuiTreeNodeFlags_AllowOverlap |
		(isExpanded ? ImGuiTreeNodeFlags_DefaultOpen : 0));

	ImGui::SameLine();

	//Status icon and color
	ImVec4 statusColor;
	const char* statusIcon;

	if (topic.isSubscribed)
	{
		statusColor = ui::colors::k_green;
		statusIcon = "S";
	}
	else
	{
		statusColor = ui::colors::k_orange;
		statusIcon = "P";

		if (!topic.lastError.empty())
		{
			statusColor = ui::colors::k_red;
			statusIcon = "F";
		}
	}

	ImGui::TextColored(statusColor, statusIcon);
	ImGui::SameLine();

	//Topic name
	ImGui::Text("%s", topic.topicFilter.c_str());

	//QoS and status on the right
	ImGui::SameLine();

	//Calculate available space for right-aligned content
	float availableWidth = ImGui::GetContentRegionAvail().x;
	float qosTextWidth = ImGui::CalcTextSize("QoS 0").x; // Estimate QoS text width
	float buttonWidth = 20.0f;
	float spacing = ImGui::GetStyle().ItemSpacing.x;

	//Position QoS text with some padding from the button
	float qosPosition = availableWidth - buttonWidth - spacing - qosTextWidth;
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + qosPosition);
	ImGui::AlignTextToFramePadding();
	ImGui::Text("QoS %d", static_cast<int>(topic.options.qos));

	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionMax().x - buttonWidth, ImGui::GetCursorPosY() + 1));

	if (ImGui::Button(BTN_LABEL(text::topics_unsubscribe_text, "unsubscribe"), ImVec2(buttonWidth, 20)))
	{
		m_model->uiData.topicToUnsubscribe = static_cast<int>(index);
		m_model->uiData.unsubscribeConfirmationText = TEXT_FORMAT(text::topics_unsubscribe_confirmation, topic.topicFilter.c_str());
		m_model->uiData.showUnsubscribeConfirmation = true;
	}

	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip(TEXT_FORMAT(text::topics_unsubscribe_confirmation, topic.topicFilter.c_str()).c_str());
	}

	//Expanded section
	if (nodeOpen)
	{
		ImGui::Indent();

		// Status details
		ImGui::Text("Status: ");
		ImGui::SameLine();
		if (topic.isSubscribed)
		{
			ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.0f, 1.0f), text::topics_subscribed);
		}
		else
		{
			ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), text::topics_pending);
		}

		if (!topic.lastError.empty())
		{
			ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), text::topics_error_str, topic.lastError.c_str());
		}

		ImGui::Text("Topic:");
		ImGui::SameLine();

		//Create a buffer for the topic filter to make it copyable
		static char topicBuffer[1024];
		strncpy(topicBuffer, topic.topicFilter.c_str(), sizeof(topicBuffer) - 1);
		topicBuffer[sizeof(topicBuffer) - 1] = '\0';

		ImGui::SetNextItemWidth(-1.0f);
		ImGui::InputText("##TopicFilterCopy", topicBuffer, sizeof(topicBuffer),
			ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_AutoSelectAll);

		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Click to select and copy topic filter");
		}

		//Topic options
		ImGui::Separator();
		ImGui::Text("Subscription Options:");
		ImGui::BulletText("QoS Level: %d", static_cast<int>(topic.options.qos));
		ImGui::BulletText("No Local: %s", topic.options.noLocal ? "Yes" : "No");
		ImGui::BulletText("Retain As Published: %s", topic.options.retainAsPublished ? "Yes" : "No");
		ImGui::BulletText("Retain Handling: %s",
			topic.options.retainHandling == cleanMqtt::mqtt::RetainHandling::SendAtSubscribe ?
			"Send at Subscribe" : "Send at Subscribe If New");

		//Resubscribe button for failed subscriptions
		if (!topic.isSubscribed)
		{
			ImGui::Separator();
			if (ImGui::Button("Retry Subscribe"))
			{
				m_model->subscribe(topic.topicFilter);
			}
		}

		ImGui::Unindent();
		ImGui::TreePop();
	}

	ImGui::PopStyleColor(2);
	ImGui::PopID();
}


void TopicsView::drawSubscribeOptions()
{
	ImGui::Indent();

	// QoS selection
	const char* qosItems[] = { "QoS 0", "QoS 1", "QoS 2" };
	ImGui::Combo("QoS Level", &m_model->uiData.selectedTopicQos, qosItems, 3);

	ImGui::Checkbox("No Local", &m_model->uiData.noLocal);
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Don't send messages published by this client back to itself");
	}

	ImGui::Checkbox("Retain As Published", &m_model->uiData.retainAsPublished);
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Keep the retain flag as it was set by the publishing client");
	}

	ImGui::Checkbox("Send Retained on Subscribe", &m_model->uiData.retainHandling);
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Send retained messages when subscribing to a topic");
	}

	ImGui::Unindent();
}

void TopicsView::drawUnsubscribeConfirmation()
{
	if (m_model->uiData.showUnsubscribeConfirmation)
	{
		ImGui::OpenPopup("Confirm Unsubscribe##UnsubConfirm");
	}

	// Center the modal
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Confirm Unsubscribe##UnsubConfirm", nullptr,
		ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
	{
		ImGui::Text("%s", m_model->uiData.unsubscribeConfirmationText.c_str());
		ImGui::NewLine();

		// Buttons
		if (ImGui::Button("Yes##ConfirmUnsub", ImVec2(80, 30)))
		{
			if (m_model->uiData.topicToUnsubscribe >= 0 &&
				m_model->uiData.topicToUnsubscribe < static_cast<int>(m_model->getSubscribedTopics().size()))
			{
				m_model->unsubscribe(static_cast<size_t>(m_model->uiData.topicToUnsubscribe));
			}
			m_model->uiData.showUnsubscribeConfirmation = false;
			m_model->uiData.topicToUnsubscribe = -1;
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("No##CancelUnsub", ImVec2(80, 30)))
		{
			m_model->uiData.showUnsubscribeConfirmation = false;
			m_model->uiData.topicToUnsubscribe = -1;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void TopicsView::handleAppliedModel(ModelPtr oldModel, ModelPtr newModel)
{

}