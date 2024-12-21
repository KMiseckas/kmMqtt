#ifndef INCLUDE_MQTTCLIENT_STYLES_H
#define INCLUDE_MQTTCLIENT_STYLES_H

#include <imgui.h>

namespace ui
{
	constexpr ImVec4 k_background{ 0.96f, 0.97f, 0.98f, 1.0f };

	constexpr ImVec4 k_border{ 0.53f, 0.19f, 0.24f, 1.0f };

	constexpr ImVec4 k_TabBackground{ 0.32f,0.54f, 0.63f, 1.0f };
	constexpr ImVec4 k_buttonBackground_Default{ 0.32f,0.54f, 0.63f, 1.0f };
	constexpr ImVec4 k_buttonBackground_Hover{ 0.25f,0.43f, 0.53f, 1.0f };
	constexpr ImVec4 k_buttonBackground_Active{ 0.20f,0.35f, 0.43f, 1.0f };
	constexpr ImVec4 k_buttonBackground_Disabled{ 0.91f,0.94f, 0.95f, 1.0f };
	constexpr ImVec4 k_buttonTextColor_Disabled{ 0.45f,0.64f,0.73f,1.0f };

	constexpr ImVec4 k_textColor_Default{ 1.0f,1.0f,1.0f,1.0f };

	inline void FirstWindowStyle()
	{
		ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 0;
	}

	inline void globalStyle()
	{
        ImGuiStyle& style = ImGui::GetStyle();

        style.WindowPadding = ImVec2(10, 10);
        style.FramePadding = ImVec2(8, 4);
        style.ItemSpacing = ImVec2(8, 8);
        style.ScrollbarSize = 15.0f;
        style.WindowBorderSize = 1.0f;
        style.ChildBorderSize = 1.0f;
        style.FrameBorderSize = 1.0f;
        style.PopupBorderSize = 1.0f;
        style.SeparatorTextBorderSize = style.WindowBorderSize;

        style.WindowRounding = 6.0f;
        style.ChildRounding = 4.0f;
        style.FrameRounding = 4.0f;
        style.PopupRounding = 4.0f;
        style.ScrollbarRounding = 4.0f;
        style.GrabRounding = 4.0f;

        ImVec4* colors = style.Colors;
        colors[ImGuiCol_Text] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);             // Dark gray text
        colors[ImGuiCol_TextDisabled] = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);    // Light gray text
        colors[ImGuiCol_WindowBg] = ImVec4(0.95f, 0.95f, 0.95f, 1.0f);     // Light background
        colors[ImGuiCol_ChildBg] = ImVec4(0.98f, 0.98f, 0.98f, 1.0f);      // Very light child background
        colors[ImGuiCol_PopupBg] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);         // White popup
        colors[ImGuiCol_Border] = ImVec4(0.85f, 0.85f, 0.85f, 1.0f);       // Light gray border
        colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);    // No shadow
        colors[ImGuiCol_FrameBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.0f);      // Light frame
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.90f, 0.90f, 0.90f, 1.0f); // Slightly darker hover
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.85f, 0.85f, 0.85f, 1.0f); // Darker active frame
        colors[ImGuiCol_TitleBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);      // Light title background
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.85f, 0.85f, 0.85f, 1.0f); // Active title
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.94f, 0.94f, 0.94f, 1.0f); // Collapsed title
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);    // Menu bar background
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);  // Scrollbar background
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.80f, 1.0f); // Scrollbar grab
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.70f, 0.70f, 0.70f, 1.0f); // Hover scrollbar
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.60f, 0.60f, 0.60f, 1.0f); // Active scrollbar
        colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.0f);    // Material blue
        colors[ImGuiCol_SliderGrab] = ImVec4(0.26f, 0.59f, 0.98f, 1.0f);   // Material blue
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.18f, 0.49f, 0.88f, 1.0f); // Darker blue
        colors[ImGuiCol_Button] = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);          // Light button
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.85f, 0.85f, 0.85f, 1.0f); // Hovered button
        colors[ImGuiCol_ButtonActive] = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);    // Active button
        colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 1.0f);       // Material blue header
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.32f, 0.63f, 0.98f, 1.0f); // Hovered header
        colors[ImGuiCol_HeaderActive] = ImVec4(0.18f, 0.49f, 0.88f, 1.0f); // Active header
        colors[ImGuiCol_Tab] = ImVec4(0.85f, 0.85f, 0.85f, 1.0f);          // Tab background
        colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.0f);   // Hovered tab
        colors[ImGuiCol_TabActive] = ImVec4(0.98f, 0.98f, 0.98f, 1.0f);    // Active tab
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.94f, 0.94f, 0.94f, 1.0f); // Unfocused tab
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.80f, 0.80f, 0.80f, 1.0f); // Active unfocused tab

	}

	inline void sessionTabBarStyle()
	{
		globalStyle();

		ImGuiStyle& style = ImGui::GetStyle();
        style.TabRounding = 4.0f;
		style.TabBarBorderSize = 0;
		style.TabBarOverlineSize = 0;
        style.TabBorderSize = 1.0f;
        style.ItemInnerSpacing = { 8,4 };
        style.ItemSpacing = ImVec2(8, 0);
        style.TabMinWidthForCloseButton = 0;
	}

    inline void sessionContentWindowStyle()
    {
        globalStyle();

        ImGuiStyle& style = ImGui::GetStyle();
        style.ChildRounding = 0;
        style.WindowBorderSize = 1;
    }

    inline void sessionContentStyle()
    {
        globalStyle();

        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowBorderSize = 1;
    }

    inline void sessionContentPanelStyle()
    {
        globalStyle();

        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowBorderSize = 1;
    }

    inline void panelTitleStyle()
    {
        globalStyle();

        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        colors[ImGuiCol_ChildBg] = ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
    }

    inline void pushPanelInputFieldStyle()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { ImGui::GetStyle().ItemSpacing.x, 2 });
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.42f, 0.42f, 0.42f, 1.0f));
    }

    inline void popPanelInputFieldStyle()
    {
        ImGui::PopStyleVar(1);
        ImGui::PopStyleColor();
    }
}

#endif //INCLUDE_MQTTCLIENT_STYLES_H 