#include "stdafx.h"
#include "PARS/Layer/DetailLayer.h"
#include "PARS/Core/Window.h"
#include "PARS/Renderer/Core/Renderer.h"

namespace PARS
{
	DetailLayer::DetailLayer(const std::string& name)
		: Layer(name)
	{
		m_WindowFlags =
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
		ResizeLayer();
	}

	void DetailLayer::Update()
	{
		ImGui::SetNextWindowPos(m_WindowPos);
		ImGui::SetNextWindowSize(m_WindowSize);

		ImGui::Begin(m_LayerName.c_str(), false, m_WindowFlags);

		{
			static ImGuiWindowFlags objectsFlags = ImGuiWindowFlags_NoScrollbar;
			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
			ImGui::BeginChild("ObjectsChild", ImVec2(m_WindowSize.x - 20.0f, m_WindowSize.y * 0.26f), true, objectsFlags);
			if (ImGui::BeginTabBar("Objects Tab"))
			{
				if (ImGui::BeginTabItem("Objects"))
				{
					UpdateObjects();
					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}
			ImGui::EndChild();
			ImGui::PopStyleVar();
		}

		{
			static ImGuiWindowFlags detailFlags = ImGuiWindowFlags_NoScrollbar;
			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
			ImGui::BeginChild("DetailChild", ImVec2(m_WindowSize.x - 20.0f, m_WindowSize.y * 0.65f), true, detailFlags);
			if (ImGui::BeginTabBar("Detail Tab"))
			{
				if (ImGui::BeginTabItem("Detail"))
				{
					UpdateDetail();
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Level Setting"))
				{
					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}
			ImGui::EndChild();
			ImGui::PopStyleVar();
		}

		{
			static ImGuiWindowFlags menuFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
			ImGui::BeginChild("Menu Child", ImVec2(m_WindowSize.x - 20.0f, m_WindowSize.y * 0.04f), true, menuFlags);
			UpdateEndMenu();
			ImGui::EndChild();
			ImGui::PopStyleVar();
		}

		ImGui::End();



		if (!m_IsOpen)
		{
			Destroy();
		}
	}

	void DetailLayer::Shutdown()
	{
		Renderer::SetClearColor(COLOR::Gray);
		f_Destroy();
	}

	void DetailLayer::ResizeLayer()
	{
		RECT rect;
		GetWindowRect(Window::GetWindowInfo()->m_hwnd, &rect);

		m_WindowSize = ImVec2(static_cast<float>(Window::GetWindowInfo()->m_LayerWidth), static_cast<float>(Window::GetWindowInfo()->m_Height));

		float x = rect.right - m_WindowSize.x - 7.5f;
		float y = rect.top + static_cast<float>(GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CXPADDEDBORDER));

		m_WindowPos = ImVec2(x, y);
	}

	void DetailLayer::UpdateObjects()
	{
		for (const auto& object : m_DetailObjects)
		{
			if (ImGui::Selectable(object.second.name.c_str(), m_SelectObjectName == object.second.name))
			{
				m_SelectObjectName = object.second.name;
				m_SelectObject = object.second;
			}
		}
	}

	void DetailLayer::UpdateDetail()
	{
		for (const auto& function : m_SelectObject.detailFunctions)
		{
			if (ImGui::TreeNode(function.treeName.c_str()))
			{
				ImGui::BeginChild(function.treeName.c_str(), ImVec2(ImGui::GetWindowWidth() * 0.9f, ImGui::GetWindowHeight() * 0.5f), true);

				function.function();

				ImGui::EndChild();
				ImGui::TreePop();
			}
		}
	}

	void DetailLayer::UpdateEndMenu()
	{
		if (ImGui::Button("Reset Level", ImVec2(180, ImGui::GetWindowHeight() * 0.6f)))
		{

		}
		ImGui::SameLine();
		if (ImGui::Button("Return To Menu", ImVec2(180, ImGui::GetWindowHeight() * 0.6f)))
		{
			m_IsOpen = false;
		}
	}

	void DetailLayer::AddObjectToLayer(DetailObject& object)
	{
		auto keyName = object.name;
		auto range = m_DetailObjects.equal_range(keyName);
		if (std::distance(range.first, range.second) != 0)
		{
			object.index = std::prev(range.second, 1)->second.index + 1;
		}
		object.name += "_" + std::to_string(object.index);

		m_DetailObjects.insert({ keyName, object });		
	}
}