#include "stdafx.h"
#include "EditorState.h"
//-----------------------------------------------------------------------------
bool EditorState::OnCreate()
{
	if( m_isCreate ) return true;

	auto& renderSystem = GetRenderSystem();
	auto& graphicsSystem = GetGraphicsSystem();

	if( !createImgui() )
		return false;

	if( !m_gridDrawer.Create(renderSystem) )
		return false;

	if( !m_collectModels.Create(renderSystem, graphicsSystem) )
		return false;

	if( !m_cursors.Create(renderSystem) )
		return false;

	m_isCreate = true;
	return true;
}
//-----------------------------------------------------------------------------
void EditorState::OnDestroy()
{
	m_cursors.Destroy();
	m_collectModels.Destroy();
	m_gridDrawer.Destroy();
	destroyImgui();
	m_isCreate = false;
}
//-----------------------------------------------------------------------------
void EditorState::OnActive()
{
	m_camera.Teleport(glm::vec3(4.0f, 5.0f, 2.0f), glm::vec3(0.0f, -1.0f, 1.0f));
	m_freeLook = false;
	m_editorMode = EditorMode::Select;
	m_currentGridHeight = 0.0f;
	m_selectEditorModeFromImGui = 0;
	m_prevPosCameraFromImGui = m_posCameraFromImGui = { m_camera.position.x, m_camera.position.y, m_camera.position.z };
	m_map.object.clear();
	m_isVisibleSelectBox = false;
}
//-----------------------------------------------------------------------------
bool EditorState::createImgui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls
	io.IniFilename = nullptr; // �� ����� ������� ���� ��������

	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	if( !ImGui_ImplGlfw_InitForOpenGL(GetWindow().GetGLFWWindow(), true) )
		return false;
	const char* glsl_version = "#version 330";
	if( !ImGui_ImplOpenGL3_Init(glsl_version) )
		return false;

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	// - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
	// io.Fonts->AddFontDefault();
	// io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
	// io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	// io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	// io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	// ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	// IM_ASSERT(font != NULL);

	return true;
}
//-----------------------------------------------------------------------------
void EditorState::destroyImgui()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
//-----------------------------------------------------------------------------