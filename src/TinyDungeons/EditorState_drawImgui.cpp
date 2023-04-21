#include "stdafx.h"
#include "EditorState.h"
//-----------------------------------------------------------------------------
void EditorState::drawImgui()
{
	if( BaseClass::IsExitApp() ) return;

	// Main Panel
	{
		const ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

		ImGui::SetNextWindowSize(ImVec2((float)GetWindowWidth(), 36.0f));
		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::Begin("Editor Menu", nullptr, flags);
		ImGui::Button("New Map");
		ImGui::SameLine();
		ImGui::Button("Open Map");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100.0f);
		ImGui::Combo("Editor Mode", (int*)&m_selectEditorModeFromImGui, "Select\0Add\0Voxel\0Sector\0Heightmap\0");
		ImGui::SameLine();
		ImGui::End();
	}

	// Info map
	{
		const ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		ImGui::SetNextWindowSize(ImVec2(150.0f, 200.0f));
		ImGui::SetNextWindowPos(ImVec2(0.0f, 36.0f));
		ImGui::Begin("Grid", nullptr, flags);

		ImGui::SetNextItemWidth(50.0f);
		ImGui::InputFloat("Height", &m_currentGridHeight);
		ImGui::SetNextItemWidth(50.0f);
		ImGui::InputFloat("Step", &m_currentGridStepHeight);
		ImGui::SetNextItemWidth(50.0f);
		ImGui::InputFloat("Size Cell", &m_currentSizeCell);
		ImGui::SetNextItemWidth(50.0f);
		ImGui::InputFloat("Size Map", &m_currentSizeMap);

		ImGui::Text(
			"Q/E : floor height\n"
			"CTRL Q/E : offset\n"
			"Shift Q/E : cell size\n"
			"QE : reset");

		ImGui::End();
	}

	// Camera Teleport Window
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		const ImS32 s32_zeroXZ = -100, s32_maxXZ = 1100;
		const ImS32 s32_zeroY = -500, s32_maxY = 500;
		ImGui::SetNextWindowSize(ImVec2(240.0f, 100.0f));
		ImGui::SetNextWindowPos(ImVec2(150.0f, 36.0f));
		ImGui::Begin("Camera", nullptr, flags);
		ImGui::SetNextItemWidth(210.0f);
		ImGui::SliderScalar("X", ImGuiDataType_S32, &m_posCameraFromImGui.x, &s32_zeroXZ, &s32_maxXZ, "%d");
		ImGui::SetNextItemWidth(210.0f);
		ImGui::SliderScalar("Y", ImGuiDataType_S32, &m_posCameraFromImGui.y, &s32_zeroY, &s32_maxY, "%d");
		ImGui::SetNextItemWidth(210.0f);
		ImGui::SliderScalar("Z", ImGuiDataType_S32, &m_posCameraFromImGui.z, &s32_zeroXZ, &s32_maxXZ, "%d");
		ImGui::End();
	}

	// Object Inspect
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		ImGui::SetNextWindowSize(ImVec2(240.0f, 200.0f));
		ImGui::SetNextWindowPos(ImVec2(GetWindowWidth() - 240.0f, 36.0f));
		ImGui::Begin("Object Inspect", nullptr, flags);
		ImGui::End();
	}

	if (m_visibleNewMapWindow) // окно создания новой карты
	{
		// TODO:
	}

	if (m_visibleLoadMapWindow) // окно открытия карты
	{
		// TODO:
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
//-----------------------------------------------------------------------------