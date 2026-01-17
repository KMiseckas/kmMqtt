// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_MQTTCLIENT_VIEWS_PUBLISHVIEW_H
#define INCLUDE_MQTTCLIENT_VIEWS_PUBLISHVIEW_H

#include "mqttClient/Views/View.h"
#include <memory>
#include <mqttClient/Model/PublishModel.h>
#include "mqttClient/Text.h"

class PublishModel;

class PublishView : public View<PublishModel>
{
public:
    void draw() override;

protected:
    void handleAppliedModel(ModelPtr oldModel, ModelPtr newModel) override;

private:
    // Main layout functions
    void drawStatsSection();
    void drawPublishSection();
    
    // Left panel functions
    void drawPublishForm();
    void drawAdvancedOptions();
    
    // Right panel functions
    void drawStatusOverview();
    void drawRecentMessages();
    
    // Popup/modal functions
    void drawClearConfirmation();
    void drawMessageDetails(const PublishedMessage& message);
    void drawMessageItem(size_t index, const PublishedMessage& message);
    void drawMessagesList();
    
    // Helper functions
    void drawMessageStatusIcon(const struct PublishedMessage& message);
    const char* getStatusText(const PublishedMessage& message);
    ImVec4 getStatusColor(const PublishedMessage& message);
    
    bool m_showWindow{ true };
    int m_selectedMessageForDetails{ -1 };
};

#endif //INCLUDE_MQTTCLIENT_VIEWS_PUBLISHVIEW_H