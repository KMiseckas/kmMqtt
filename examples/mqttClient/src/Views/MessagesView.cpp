// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <mqttClient/Views/MessagesView.h>
#include <mqttClient/Model/MessagesModel.h>
#include <mqttClient/Text.h>
#include <mqttClient/Styles.h>

void MessagesView::draw()
{
    if (!getModel())
    {
        return;
    }

    drawMessages();
    drawMessageDetailsModal();
    drawClearConfirmation();
}

void MessagesView::drawMessages()
{
    const auto& model = getModel();
    
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
    
    float availableWidth{ ImGui::GetContentRegionAvail().x };
    float buttonWidth{ (availableWidth - 8.0f) * 0.5f };
    
    //Received button
    bool showingReceived{ model->uiData.messageViewMode == 0 };
    if (showingReceived)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ui::colors::k_blue);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.6f, 1.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ui::colors::k_grey);
    }
    
    if (ImGui::Button(text::messages_column_received, ImVec2(buttonWidth, 0)))
    {
        model->uiData.messageViewMode = 0;
    }
    ImGui::PopStyleColor(3);
    
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 4.0f);
    
    //Sent messages button
    bool showingSent = (model->uiData.messageViewMode == 1);
    if (showingSent)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ui::colors::k_green);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 1.0f, 0.6f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ui::colors::k_grey);
    }
    
    if (ImGui::Button(text::messages_column_sent, ImVec2(buttonWidth, 0)))
    {
        model->uiData.messageViewMode = 1;
    }
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(2);
    
    auto height{ ImGui::GetContentRegionAvail().y - 30};

    // Display the selected message type
    if (showingReceived)
    {
        drawReceivedMessages(height);
    }
    else
    {
        drawSentMessages(height);
    }

    if (ImGui::Button(text::messages_view_clear_button, ImVec2(0, 22)))
    {
        model->uiData.showClearConfirmation = true;
    }
}

void MessagesView::drawReceivedMessages(float height)
{
    const auto& msgs{ getModel()->getAllMessages() };
    bool hasReceivedMessages = false;

    if (ImGui::BeginChild("ReceivedMessages", ImVec2(0, height), true))
    {
        for (size_t i = 0; i < msgs.size(); ++i)
        {
            if (msgs[i].type == MessageType::RECEIVED)
            {
                hasReceivedMessages = true;
                drawMessageItem(msgs[i], i);
            }
        }

        if (!hasReceivedMessages)
        {
            ImGui::TextColored(ui::colors::k_grey, "No received messages");
        }
    }
    ImGui::EndChild();
}

void MessagesView::drawSentMessages(float height)
{
    const auto& msgs{ getModel()->getAllMessages() };
    bool hasSentMessages = false;

    if (ImGui::BeginChild("SentMessages", ImVec2(0, height), true))
    {
        for (size_t i = 0; i < msgs.size(); ++i)
        {
            if (msgs[i].type == MessageType::SENT)
            {
                hasSentMessages = true;
                drawMessageItem(msgs[i], i);
            }
        }

        if (!hasSentMessages)
        {
            ImGui::TextColored(ui::colors::k_grey, "No sent messages");
        }
    }
    ImGui::EndChild();
}

void MessagesView::drawMessageItem(const MqttMessage& message, size_t globalIndex)
{
    ImGui::PushID(static_cast<int>(globalIndex));

    ImVec4 statusColor{ getMessageStatusColor(message) };

    ImGui::PushStyleColor(ImGuiCol_Border, statusColor);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);

    ImGui::BeginGroup();
    
    //Clickable message item   
    ImVec2 itemSize(ImGui::GetContentRegionAvail().x - 10, 40);
    if (ImGui::Selectable("##MessageItem", false, ImGuiSelectableFlags_None, itemSize))
    {
        getModel()->uiData.selectedMessageIndex = static_cast<int>(globalIndex);
        getModel()->uiData.showMessageDetails = true;
    }

    //Get the item rect for overlay positioning
    ImVec2 itemMin{ ImGui::GetItemRectMin() };
    ImVec2 itemMax{ ImGui::GetItemRectMax() };
    
    //Draw overlay content using the window draw list
    ImDrawList* drawList{ ImGui::GetWindowDrawList() };
    
    //Calculate positions relative to item rect
    ImVec2 topicPos(itemMin.x + 5, itemMin.y + 2);
    ImVec2 payloadPos(itemMin.x + 20, itemMin.y + 18);
    ImVec2 sentStatusPos(itemMin.x + 5, itemMin.y + 34);
    
    std::string displayTopic = message.topic;
    if (displayTopic.length() > 25) 
    {
        displayTopic = displayTopic.substr(0, 22) + "...";
    }
    drawList->AddText(topicPos, IM_COL32(60, 60, 60, 255), displayTopic.c_str());
    
    //Draw timestamp (right-aligned)
    std::string timeStr{ formatTimestamp(message.timestamp) };
    float timeWidth{ ImGui::CalcTextSize(timeStr.c_str()).x };
    ImVec2 timePos(itemMax.x - timeWidth - 10, topicPos.y);
    drawList->AddText(timePos, ImGui::ColorConvertFloat4ToU32(ui::colors::k_grey), timeStr.c_str());
    
    //Draw payload preview
    std::string payloadPreview{ truncatePayload(message.payload, 60) };
    drawList->AddText(payloadPos, IM_COL32(120, 120, 120, 255), payloadPreview.c_str());
    
    //Status text for sent messages
    if (message.type == MessageType::SENT) 
    {
		drawList->AddText(sentStatusPos, ImGui::ColorConvertFloat4ToU32(statusColor), getMessageStatusText(message));
    }
    
    ImGui::EndGroup();
    
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::PopID();
    
    //Add spacing for next item
    ImGui::Dummy(ImVec2(0, 5)); // 5 pixel spacing between items
}

ImVec4 MessagesView::getMessageStatusColor(const MqttMessage& message)
{
    if (message.type == MessageType::RECEIVED)
    {
        return ui::colors::k_blue;
    }
    else
    {
        switch (message.sentStatus)
        {
        case SentMessageStatus::PENDING:
            return ui::colors::k_orange;
        case SentMessageStatus::ACKNOWLEDGED:
            return ui::colors::k_green;
        case SentMessageStatus::FAILED:
            return ui::colors::k_red;
        default:
            return ui::colors::k_grey;
        }
    }
}

const char* MessagesView::getMessageStatusText(const MqttMessage& message)
{
    if (message.type == MessageType::RECEIVED)
    {
        return text::messages_status_received;
    }
    else
    {
        switch (message.sentStatus)
        {
        case SentMessageStatus::PENDING:
            return text::messages_status_sent_pending;
        case SentMessageStatus::ACKNOWLEDGED:
            return text::messages_status_sent_acknowledged;
        case SentMessageStatus::FAILED:
            return text::messages_status_sent_failed;
        default:
            return "Unknown";
        }
    }
}

void MessagesView::drawMessageDetailsModal()
{
    if (!getModel()->uiData.showMessageDetails || getModel()->uiData.selectedMessageIndex < 0)
    {
        return;
    }

    const auto& msgs{ getModel()->getAllMessages() };
    if (static_cast<size_t>(getModel()->uiData.selectedMessageIndex) >= msgs.size())
    {
        getModel()->uiData.showMessageDetails = false;
        return;
    }

    const auto& message = msgs[getModel()->uiData.selectedMessageIndex];

    ImGui::OpenPopup(text::messages_details_title);

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(600, 450), ImGuiCond_Appearing);

    if (ImGui::BeginPopupModal(text::messages_details_title, nullptr))
    {
        //Message type and status
        ImGui::Text(text::messages_details_type_label);
        ImGui::SameLine();
        ImVec4 statusColor = getMessageStatusColor(message);
        ImGui::TextColored(statusColor, "%s", message.type == MessageType::RECEIVED ? text::messages_type_received : text::messages_type_sent);

        if (message.type == MessageType::SENT)
        {
            ImGui::SameLine();
            ImGui::Text("-");
            ImGui::SameLine();
            ImGui::TextColored(statusColor, "%s", getMessageStatusText(message));
        }

        ImGui::Separator();

        //Topic
        ImGui::Text(text::messages_details_topic_label);
        ImGui::SameLine();
        static char topicBuffer[1024];
        strncpy(topicBuffer, message.topic.c_str(), sizeof(topicBuffer) - 1);
        topicBuffer[sizeof(topicBuffer) - 1] = '\0';

        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputText("##TopicCopy", topicBuffer, sizeof(topicBuffer),
            ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_AutoSelectAll);

        //Timestamp
        ImGui::Text(text::messages_details_timestamp_label);
        ImGui::SameLine();
        auto time_t = std::chrono::system_clock::to_time_t(message.timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        ImGui::Text("%s", ss.str().c_str());

        //QoS and Retain info
        if (message.type == MessageType::RECEIVED)
        {
            ImGui::Text(text::messages_details_qos_label);
            ImGui::SameLine();
            ImGui::Text("%d", static_cast<int>(message.receivedQos));

            ImGui::SameLine();
            ImGui::Text(text::messages_details_retain_label);
            ImGui::SameLine();
            ImGui::Text("%s", message.receivedRetain ? "Yes" : "No");
        }
        else
        {
            ImGui::Text(text::messages_details_qos_label);
            ImGui::SameLine();
            ImGui::Text("%d", static_cast<int>(message.sentOptions.qos));

            ImGui::SameLine();
            ImGui::Text(text::messages_details_retain_label);
            ImGui::SameLine();
            ImGui::Text("%s", message.sentOptions.retain ? "Yes" : "No");
        }

        //Payload
        ImGui::Text(TEXT_FORMAT(text::messages_details_payload_label, message.payload.length()).c_str());

        //Scrollable text area for payload
        if (ImGui::BeginChild("PayloadContent", ImVec2(-1, -50), true))
        {
            ImGui::TextWrapped("%s", message.payload.c_str());
        }
        ImGui::EndChild();

        //Error message for failed sent messages
        if (message.type == MessageType::SENT && message.sentStatus == SentMessageStatus::FAILED && !message.lastError.empty())
        {
            ImGui::Separator();
            ImGui::TextColored(ui::colors::k_red, "Error: %s", message.lastError.c_str());
        }

        //Close button
        if (ImGui::Button(text::messages_details_close_button, ImVec2(-1, 0)))
        {
            getModel()->uiData.showMessageDetails = false;
            getModel()->uiData.selectedMessageIndex = -1;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    else
    {
        getModel()->uiData.showMessageDetails = false;
        getModel()->uiData.selectedMessageIndex = -1;
    }
}

void MessagesView::drawClearConfirmation()
{
    if (getModel()->uiData.showClearConfirmation)
    {
        ImGui::OpenPopup(text::messages_clear_confirmation_title);
    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal(text::messages_clear_confirmation_title, nullptr,
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
    {
        ImGui::Text(text::messages_clear_confirmation_message1);
        ImGui::Text(text::messages_clear_confirmation_message2);
        ImGui::NewLine();

        //Buttons
        if (ImGui::Button(text::messages_clear_confirmation_yes_button, ImVec2(120, 30)))
        {
            getModel()->clearAllMessages();
            getModel()->uiData.showClearConfirmation = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button(text::messages_clear_confirmation_cancel_button, ImVec2(80, 30)))
        {
            getModel()->uiData.showClearConfirmation = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

std::string MessagesView::truncatePayload(const std::string& payload, size_t maxLength)
{
    if (payload.length() <= maxLength)
    {
        return payload;
    }

    std::string truncated = payload.substr(0, maxLength - 3);

    //Replace newlines with spaces for preview
    for (char& c : truncated)
    {
        if (c == '\n' || c == '\r' || c == '\t')
        {
            c = ' ';
        }
    }

    return truncated + "...";
}

std::string MessagesView::formatTimestamp(const std::chrono::system_clock::time_point& timestamp)
{
    auto time_t = std::chrono::system_clock::to_time_t(timestamp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
    return ss.str();
}

void MessagesView::handleAppliedModel(ModelPtr oldModel, ModelPtr newModel)
{
    //Reset UI state when model changes
    if (newModel)
    {
        newModel->uiData.selectedMessageIndex = -1;
        newModel->uiData.showMessageDetails = false;
        newModel->uiData.showClearConfirmation = false;
    }
}