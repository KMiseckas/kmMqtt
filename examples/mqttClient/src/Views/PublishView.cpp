#include <mqttClient/Views/PublishView.h>
#include <mqttClient/Model/PublishModel.h>
#include <mqttClient/Text.h>
#include <imgui.h>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <imgui_internal.h>

void PublishView::draw()
{
    if (!getModel())
    {
        return;
    }

    // Left column (50% - main content)
    if (ImGui::BeginChild("LeftColumn", ImVec2(ImGui::GetContentRegionAvail().x * 0.6f, 0), false))
    {
        drawPublishSection();
    }
    ImGui::EndChild();

    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();

    // Middle column (25% - empty for now)
    if (ImGui::BeginChild("RightColumn", ImVec2(ImGui::GetContentRegionAvail().x, 0), false))
    {
        drawStatsSection();
        ImGui::Separator();
        drawMessagesList();
        drawClearConfirmation();
    }
    ImGui::EndChild();
}

void PublishView::drawStatsSection()
{
    const auto& model = getModel();
    
    ImGui::Text(text::publish_stats_messages_total, model->getPublishedMessages().size());
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), text::publish_stats_pending, model->getPendingCount());
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.0f, 1.0f), text::publish_stats_success, model->getSuccessCount());
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), text::publish_stats_failed, model->getFailedCount());
}

void PublishView::drawPublishSection()
{
    ImGui::Text(text::publish_form_title);
    
    //Topic input
    ImGui::InputTextWithHint(text::publish_form_topic_label,
        text::publish_form_topic_hint, 
        getModel()->uiData.topicBuffer,
        sizeof(getModel()->uiData.topicBuffer));
    
    //Payload input
    ImGui::InputTextMultiline(text::publish_form_payload_label,
        getModel()->uiData.payloadBuffer, 
        sizeof(getModel()->uiData.payloadBuffer),
        ImVec2(-1, ImGui::GetContentRegionAvail().y * 0.7f));
    
    //Basic options
    const char* qosItems[] = { text::publish_qos_0, text::publish_qos_1, text::publish_qos_2 };
    ImGui::SetNextItemWidth(100);
    ImGui::Combo(text::publish_form_qos_label, &getModel()->uiData.selectedQos, qosItems, IM_ARRAYSIZE(qosItems));
    
    ImGui::SameLine();
    ImGui::Checkbox(text::publish_form_retain_label, &getModel()->uiData.retain);
    
    ImGui::SameLine();
    if (ImGui::Button(text::publish_form_advanced_button))
    {
        getModel()->uiData.showAdvancedOptions = !getModel()->uiData.showAdvancedOptions;
    }
    
    //Publish button
    bool canPublish = strlen(getModel()->uiData.topicBuffer) > 0;
    if (!canPublish)
    {
        ImGui::BeginDisabled();
    }
    
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 120.0f);

    if (ImGui::Button(text::publish_form_publish_button, ImVec2(120, 0)))
    {
        getModel()->publish(getModel()->uiData.topicBuffer, { getModel()->uiData.payloadBuffer, strlen(getModel()->uiData.payloadBuffer) });

        memset(getModel()->uiData.payloadBuffer, 0, sizeof(getModel()->uiData.payloadBuffer));
    }

    if (!canPublish)
    {
        ImGui::EndDisabled();
    }

    //Advanced options
    if (getModel()->uiData.showAdvancedOptions)
    {
        drawAdvancedOptions();
    }
}

void PublishView::drawAdvancedOptions()
{
    ImGui::Separator();
    ImGui::Text(text::publish_advanced_title);
    
    //Response topic
    ImGui::InputTextWithHint(text::publish_advanced_response_topic_label,
        text::publish_advanced_response_topic_hint, 
        getModel()->uiData.responseTopicBuffer,
        sizeof(getModel()->uiData.responseTopicBuffer));
    
    //Topic alias
    ImGui::InputInt(text::publish_advanced_topic_alias_label, &getModel()->uiData.topicAlias);
    
    //Message expiry
    ImGui::Checkbox(text::publish_advanced_message_expiry_label, &getModel()->uiData.addMessageExpiryInterval);

    if (getModel()->uiData.addMessageExpiryInterval)
    {
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        ImGui::InputInt("##ExpiryInterval", &getModel()->uiData.messageExpiryInterval);
        ImGui::SameLine();
        ImGui::Text(text::publish_advanced_message_expiry_seconds);
    }
    
    //Payload format
    const char* formatItems[] = { text::publish_format_utf8, text::publish_format_binary };
    ImGui::Combo(text::publish_advanced_payload_format_label, &getModel()->uiData.payloadFormatIndicator, formatItems, IM_ARRAYSIZE(formatItems));
    
    //Correlation data
    ImGui::InputTextWithHint(text::publish_advanced_correlation_data_label,
        text::publish_advanced_correlation_data_hint, 
        getModel()->uiData.correlationDataBuffer,
        sizeof(getModel()->uiData.correlationDataBuffer));
}

void PublishView::drawMessagesList()
{
    ImGui::Text(text::publish_messages_title);
    
    // Filter dropdown
    const char* filterItems[] = { text::publish_filter_all, text::publish_filter_pending, text::publish_filter_sent, text::publish_filter_acknowledged, text::publish_filter_failed };
    ImGui::SetNextItemWidth(150);
    ImGui::Combo(text::publish_messages_filter_label, &getModel()->uiData.statusFilter, filterItems, IM_ARRAYSIZE(filterItems));
    
    ImGui::SameLine();
    if (ImGui::Button(text::publish_form_clear_all_button, ImVec2(80, 0)))
    {
        getModel()->uiData.showClearConfirmation = true;
    }

    const auto& messages = getModel()->getPublishedMessages();
    
    if (messages.empty())
    {
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), text::publish_messages_no_messages);
        return;
    }
    
    // Create a scrollable region
    if (ImGui::BeginChild("MessagesScrollRegion", ImVec2(0, -1), true))
    {
        // Render each message
        for (size_t i = 0; i < messages.size(); ++i)
        {
            const auto& message = messages[i];
            
            // Apply filter
            if (getModel()->uiData.statusFilter != 0)
            {
                PublishMessageStatus filterStatus = static_cast<PublishMessageStatus>(getModel()->uiData.statusFilter - 1);
                if (message.status != filterStatus)
                    continue;
            }
            
            drawMessageItem(i, message);
        }
    }
    ImGui::EndChild();
}

void PublishView::drawMessageItem(size_t index, const PublishedMessage& message)
{
    ImGui::PushID(static_cast<int>(index));
    
    const bool isExpanded = (getModel()->uiData.expandedMessageIndex == static_cast<int>(index));
    
    ImVec4 statusColor;
    const char* statusIcon;
    const char* statusText;
    
    switch (message.status)
    {
    case PublishMessageStatus::PENDING:
        statusColor = ImVec4(0.6f, 0.6f, 0.6f, 1.0f); // Gray
        statusIcon = "P";
        statusText = text::publish_status_pending;
        break;
    case PublishMessageStatus::SENT:
        statusColor = ImVec4(1.0f, 0.8f, 0.0f, 1.0f); // Orange
        statusIcon = "S";
        statusText = text::publish_status_sent;
        break;
    case PublishMessageStatus::ACKNOWLEDGED:
        statusColor = ImVec4(0.0f, 0.8f, 0.0f, 1.0f); // Green
        statusIcon = "A";
        statusText = text::publish_status_acknowledged;
        break;
    case PublishMessageStatus::FAILED:
        statusColor = ImVec4(1.0f, 0.2f, 0.2f, 1.0f); // Red
        statusIcon = "F";
        statusText = text::publish_status_failed;
        break;
    }
    
    // Clickable message item
    ImGui::AlignTextToFramePadding();
    
    bool nodeOpen = ImGui::TreeNodeEx("##MessageNode", 
        ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanFullWidth | 
        ImGuiTreeNodeFlags_AllowOverlap |
        (isExpanded ? ImGuiTreeNodeFlags_DefaultOpen : 0));
    
    ImGui::SameLine();
    
    // Status icon
    ImGui::TextColored(statusColor, "%s", statusIcon);
    ImGui::SameLine();
    
    // Topic name (truncated if too long)
    std::string displayTopic = message.topic;
    if (displayTopic.length() > 30)
    {
        displayTopic = displayTopic.substr(0, 27) + "...";
    }
    ImGui::Text("%s", displayTopic.c_str());
    
    // QoS and timestamp on the right
    ImGui::SameLine();
    float availableWidth = ImGui::GetContentRegionAvail().x;
    float statusWidth = ImGui::CalcTextSize(statusText).x;
    float qosWidth = ImGui::CalcTextSize(text::publish_qos_0).x;
    float spacing = ImGui::GetStyle().ItemSpacing.x;
    
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + availableWidth - statusWidth - qosWidth - spacing * 3);
    ImGui::Text(text::publish_details_qos, static_cast<int>(message.options.qos));
    
    ImGui::SameLine();
    ImGui::TextColored(statusColor, "%s", statusText);
    
    // Expanded section
    if (nodeOpen)
    {
        drawMessageDetails(message);
        ImGui::TreePop();
    }
    
    ImGui::PopID();
}

void PublishView::drawMessageDetails(const PublishedMessage& message)
{
    ImGui::Indent();
    
    // Topic (copyable)
    ImGui::Text(text::publish_details_topic_label);
    ImGui::SameLine();
    static char topicBuffer[1024];
    strncpy(topicBuffer, message.topic.c_str(), sizeof(topicBuffer) - 1);
    topicBuffer[sizeof(topicBuffer) - 1] = '\0';
    
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::InputText("##TopicCopy", topicBuffer, sizeof(topicBuffer),
        ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_AutoSelectAll);
    
    // Payload preview
    ImGui::Text(text::publish_details_payload_label, message.payload.length());
    std::string displayPayload = message.payload;
    if (displayPayload.length() > 200)
    {
        displayPayload = displayPayload.substr(0, 197) + "...";
    }
    ImGui::TextWrapped("%s", displayPayload.c_str());
    
    // Publish options
    ImGui::Separator();
    ImGui::Text(text::publish_details_options_title);
    ImGui::BulletText(text::publish_details_qos, static_cast<int>(message.options.qos));
    ImGui::BulletText(message.options.retain ? text::publish_details_retain_yes : text::publish_details_retain_no);
    ImGui::BulletText(message.options.payloadFormatIndicator == cleanMqtt::mqtt::PayloadFormatIndicator::UTF8 ? text::publish_details_format_utf8 : text::publish_details_format_binary);
    
    if (!message.options.responseTopic.empty())
    {
        ImGui::BulletText(text::publish_details_response_topic, message.options.responseTopic.c_str());
    }
    
    if (message.options.topicAlias > 0)
    {
        ImGui::BulletText(text::publish_details_topic_alias, message.options.topicAlias);
    }
    
    if (message.options.addMessageExpiryInterval)
    {
        ImGui::BulletText(text::publish_details_message_expiry, message.options.messageExpiryInterval);
    }
    
    // Timestamp
    auto time_t = std::chrono::system_clock::to_time_t(message.timestamp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ImGui::BulletText(text::publish_details_timestamp, ss.str().c_str());
    
    // Error message if failed
    if (!message.lastError.empty())
    {
        ImGui::Separator();
        ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), text::publish_details_error, message.lastError.c_str());
    }
    
    ImGui::Unindent();
}

void PublishView::drawClearConfirmation()
{
    if (getModel()->uiData.showClearConfirmation)
    {
        ImGui::OpenPopup(text::publish_clear_confirmation_title);
    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    
    if (ImGui::BeginPopupModal(text::publish_clear_confirmation_title, nullptr, 
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
    {
        ImGui::Text(text::publish_clear_confirmation_message1);
        ImGui::Text(text::publish_clear_confirmation_message2);
        ImGui::NewLine();
        
        // Buttons
        if (ImGui::Button(text::publish_clear_confirmation_yes_button, ImVec2(120, 30)))
        {
            getModel()->clearAllMessages();
            getModel()->uiData.showClearConfirmation = false;
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button(text::publish_clear_confirmation_cancel_button, ImVec2(80, 30)))
        {
            getModel()->uiData.showClearConfirmation = false;
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    }
}

void PublishView::handleAppliedModel(ModelPtr oldModel, ModelPtr newModel)  
{
    //Nothing
}