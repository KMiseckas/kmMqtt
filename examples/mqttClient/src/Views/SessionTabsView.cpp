#include "mqttClient/Views/SessionTabsView.h"
#include <mqttClient/Styles.h>
#include <cstdint>
#include <mqttClient/Fonts.h>
#include <mqttClient/Text.h>


void SessionTabsView::draw()
{
	ui::sessionTabBarStyle();

	const std::size_t numberSessions = m_model->getNumberOfSession();

	if (ImGui::BeginTabBar("SessionTabBar", ImGuiTabBarFlags_AutoSelectNewTabs))
	{
		std::vector<std::uint8_t> toRemove;

		if (numberSessions > 0)
		{
			for (std::uint8_t i{ 0 }; i < numberSessions; ++i)
			{
				auto sessionModel = m_model->getSessionModel(i);
				const std::string imGuiItemLabel{ sessionModel->getName() + "##" + std::to_string(i) };

				bool active = true;

				if (ImGui::BeginTabItem(imGuiItemLabel.c_str(), &active))
				{
					if (!active)
					{
						toRemove.push_back(i);
					}

					if (i != m_model->getOpenSessionIndex())
					{
						m_model->setOpenSession(i);
					}

					ImGui::EndTabItem();
				}
			}

			for (const auto index : toRemove)
			{
				m_model->removeSession(index);
			}
		}

		if (numberSessions != m_model->k_maxSessionsAllowed)
		{
			if (ImGui::TabItemButton("+"))
			{
				m_model->addSession();
			}
		}
		ImGui::EndTabBar();
	}

	ui::sessionContentWindowStyle();
	if (ImGui::BeginChild("Session Content Area", { 0,0 }, ImGuiChildFlags_Borders))
	{
		if (numberSessions > 0)
		{
			m_activeSessionView.draw();
		}
		else
		{
			drawDefaultView();
		}
	}
	ImGui::EndChild();
}

void SessionTabsView::drawDefaultView()
{
	const ImVec2 area{ ImGui::GetContentRegionAvail() };
	const ImVec2 center = { area.x * 0.5f,  area.y * 0.5f };
	const ImVec2 textSize{ ImGui::CalcTextSize(text::add_session) };

	ImGui::SetCursorPos({ center.x - (textSize.x * 0.5f), center.y - (textSize.y * 0.5f) });
	ImGui::Text(text::add_session);
}

void SessionTabsView::handleAppliedModel(ModelPtr oldModel, ModelPtr newModel)
{
	oldModel->onOpenSessionChangedEvent.remove(m_onSessionChangedEventHandle);

	if (newModel != nullptr)
	{
		m_onSessionChangedEventHandle = newModel->onOpenSessionChangedEvent.add([&](SessionModelPtr model)
			{
					m_activeSessionView.setModel(model);
			});
	}
}