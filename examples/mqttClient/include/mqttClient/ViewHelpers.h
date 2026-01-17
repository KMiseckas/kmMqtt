// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_MQTTCLIENT_VIEWHELPERS_H
#define INCLUDE_MQTTCLIENT_VIEWHELPERS_H

#include <imgui.h>

namespace viewHelpers 
{
	inline bool beginPanel(const char* label, const ImVec2& size = { 0,0 }, ImGuiChildFlags childFlags = 0, ImGuiWindowFlags windowFlags = 0)
	{
		if (ImGui::BeginChild(label, size, ImGuiChildFlags_Borders | childFlags, windowFlags))
		{
			const float padding = ImGui::GetStyle().WindowPadding.x;

			ImGui::SeparatorText(label);

			return true;
		}
	
		return false;
	}

	inline void endPanel()
	{
		ImGui::EndChild();
	}
}

#endif //INCLUDE_MQTTCLIENT_VIEWHELPERS_H 
