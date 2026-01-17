// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <mqttClient/Views/OutputView.h>
#include <mqttClient/Model/OutputModel.h>
#include <mqttClient/Text.h>
#include <mqttClient/Styles.h>
#include <imgui.h>
#include <sstream>

void OutputView::draw()
{
	if (!getModel())
	{
		return;
	}

	drawLogOutput();
	drawClearConfirmation();
}

void OutputView::drawLogOutput()
{
	const auto& model{ getModel() };
	const auto& logs{ model->GetAllLogs() };

	//Copy All
	if (ImGui::Button(text::output_copy_all_button, ImVec2(0, 0)))
	{
		copyAllLogsToClipboard();
	}

	ImGui::SameLine();

	//Clear Logs
	if (ImGui::Button(text::output_clear_button, ImVec2(0, 0)))
	{
		model->uiData.showClearConfirmation = true;
	}

	ImGui::SameLine();

	//Log to File
	bool logToFile{ model->isFileLoggingEnabled()};
	if (ImGui::Checkbox(text::output_enable_logging_to_file, &logToFile))
	{
		if (logToFile)
		{
			model->enableFileLogging("mqtt_client_log.txt");
		}
		else
		{
			model->disableFileLogging();
		}
	}

	//Scrollable Area
	auto height = ImGui::GetContentRegionAvail().y;

	if (ImGui::BeginChild("LogOutput", ImVec2(0, height), true, ImGuiWindowFlags_HorizontalScrollbar))
	{
		if (model->getLogCount() == 0)
		{
			ImGui::TextColored(ui::colors::k_grey, text::output_no_logs);
		}
		else
		{
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
			
			for (std::size_t i = 0; i < model->getLogCount(); ++i)
			{
				const auto& logEntry{ logs[i] };
				ImGui::TextColored(getLogLevelColor(logEntry.logLevel), "%s", logEntry.logEntry.c_str());
			}

			ImGui::PopFont();

			if (model->uiData.autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			{
				ImGui::SetScrollHereY(1.0f);
			}
		}
	}
	ImGui::EndChild();
}

void OutputView::drawClearConfirmation()
{
	if (getModel()->uiData.showClearConfirmation)
	{
		ImGui::OpenPopup(text::output_clear_confirmation_title);
	}

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal(text::output_clear_confirmation_title, nullptr,
		ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
	{
		ImGui::Text(text::output_clear_confirmation_message1);
		ImGui::Text(text::output_clear_confirmation_message2);
		ImGui::NewLine();

		if (ImGui::Button(text::output_clear_confirmation_yes_button, ImVec2(120, 30)))
		{
			getModel()->clearLogs();
			getModel()->uiData.showClearConfirmation = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button(text::output_clear_confirmation_cancel_button, ImVec2(80, 30)))
		{
			getModel()->uiData.showClearConfirmation = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void OutputView::copyAllLogsToClipboard()
{
	const auto& logs{ getModel()->GetAllLogs() };
	
	if (getModel()->getLogCount() == 0)
	{
		return;
	}

	std::stringstream ss;
	for (std::size_t i = 0; i < getModel()->getLogCount(); ++i)
	{
		ss << logs[i].logEntry << "\n";
	}

	ImGui::SetClipboardText(ss.str().c_str());
}

ImVec4 OutputView::getLogLevelColor(kmMqtt::LogLevel logLevel) const
{
	if (logLevel == kmMqtt::LogLevel::Error || logLevel == kmMqtt::LogLevel::Fatal)
	{
		return ui::colors::k_red;
	}
	else if (logLevel == kmMqtt::LogLevel::Warning)
	{
		return ui::colors::k_yellow;
	}

	return ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
}

void OutputView::handleAppliedModel(ModelPtr oldModel, ModelPtr newModel)
{
	//Reset UI state when model changes
	if (newModel)
	{
		newModel->uiData.showClearConfirmation = false;
		newModel->uiData.autoScroll = true;
	}
}