// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_MQTTCLIENT_VIEWS_MESSAGESVIEW_H
#define INCLUDE_MQTTCLIENT_VIEWS_MESSAGESVIEW_H

#include "mqttClient/Views/View.h"
#include <memory>
#include <string>
#include <ctime>
#include <imgui.h>
#include <iomanip>
#include <sstream>
#include <chrono>

class MessagesModel;
struct MqttMessage;

class MessagesView : public View<MessagesModel>
{
public:
    void draw() override;

protected:
    void handleAppliedModel(ModelPtr oldModel, ModelPtr newModel) override;

private:
    // Main layout functions
    void drawMessages();
    void drawReceivedMessages(float height);
    void drawSentMessages(float height);

    // Message rendering
    void drawMessageItem(const MqttMessage& message, size_t globalIndex);
    void drawMessagePreview(const MqttMessage& message);
    ImVec4 getMessageStatusColor(const MqttMessage& message);
    const char* getMessageStatusText(const MqttMessage& message);

    // Popup/modal functions
    void drawMessageDetailsModal();
    void drawClearConfirmation();

    // Helper functions
    std::string truncatePayload(const std::string& payload, size_t maxLength = 100);
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp);

    bool m_showWindow{ true };
};

#endif //INCLUDE_MQTTCLIENT_VIEWS_MESSAGESVIEW_H