// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_MQTTCLIENT_VIEWS_VIEW_H
#define INCLUDE_MQTTCLIENT_VIEWS_VIEW_H

#include <mqttClient/Events/DirectEvent.h>
#include <mqttClient/Text.h>
#include <mqttClient/Fonts.h>
#include <mqttClient/Styles.h>

#include <imgui.h>
#include <utility>
#include <memory>

template<class TModel>
class View 
{
public:
	using ModelPtr = std::shared_ptr<TModel>;

	View() noexcept
	{
	}

	virtual ~View() 
	{
	}

	virtual void draw() = 0;

	inline void setModel(ModelPtr model) noexcept
	{
		if (model != m_model)
		{
			ModelPtr oldModel = m_model;
			m_model = model;
			handleAppliedModel(oldModel, m_model);
		}
	}

	inline ModelPtr getModel() const noexcept
	{
		return m_model;
	}

	inline void reset() noexcept
	{
		//TODO
	}

protected:
	virtual void handleAppliedModel(ModelPtr oldModel, ModelPtr newModel) = 0;

	ModelPtr m_model;
};

#endif //INCLUDE_MQTTCLIENT_VIEWS_VIEW_H 