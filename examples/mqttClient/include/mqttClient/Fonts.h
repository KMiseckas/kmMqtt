#ifndef INCLUDE_MQTTCLIENT_FONTS_H
#define INCLUDE_MQTTCLIENT_FONTS_H

#include <imgui.h>

namespace fonts
{
    static ImFont* defaultFont{ nullptr };

	inline void initFonts()
	{
        ImGuiIO& io = ImGui::GetIO();

        defaultFont = io.Fonts->AddFontFromFileTTF(DROIDSANS_FONT_PATH, 14.0f);
	}
}

#endif //INCLUDE_MQTTCLIENT_FONTS_H