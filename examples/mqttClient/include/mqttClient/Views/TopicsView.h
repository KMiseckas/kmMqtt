// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_MQTTCLIENT_VIEWS_TOPICSVIEW_H
#define INCLUDE_MQTTCLIENT_VIEWS_TOPICSVIEW_H

#include "mqttClient/Views/View.h"
#include <memory>

class TopicsModel;

class TopicsView : public View<TopicsModel>
{
public:
    void draw() override;

protected:
    void handleAppliedModel(ModelPtr oldModel, ModelPtr newModel) override;

private:
    void drawAddTopicSection();
    void drawTopicsList();
    void drawSubscribeOptions();
    void drawTopicItem(size_t index, const struct SubscribedTopic& topic);
    void drawUnsubscribeConfirmation();

    bool m_showWindow{ true };
};

#endif //INCLUDE_MQTTCLIENT_VIEWS_TOPICSVIEW_H