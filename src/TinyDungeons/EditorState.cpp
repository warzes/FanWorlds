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
	m_mode = EditorMode::Select;
	m_currentGridHeight = 0.0f;
	m_selectMode = 0;
	m_oldCamTelData = m_camTelData = { m_camera.position.x, m_camera.position.y, m_camera.position.z };

	m_map.object.clear();
}
//-----------------------------------------------------------------------------
void EditorState::OnUpdate(float deltaTime)
{
	Input& input = GetInput();

	selectMode();

	if( updateImgui() ) return; // имгуи перехватил события?

	cameraUpdate(deltaTime);

	if( input.IsKeyDown(Input::KEY_ESCAPE) )
	{
		BaseClass::ExitRequest();
		return;
	}

	modGrid();


	if( input.IsKeyDown(Input::KEY_LEFT_SHIFT) )
	{
		m_objPosition = {m_cursors.GetPosX(), m_currentGridHeight, m_cursors.GetPosZ()};
	}
	else
		m_objPosition = {
			floor((m_cursors.GetPosX() + 0.5f) / m_currentSizeCell) * m_currentSizeCell,
			m_currentGridHeight,
			floor((m_cursors.GetPosZ() + 0.5f) / m_currentSizeCell) * m_currentSizeCell };

	const float wheel = input.GetMouseWheelMove();
	if( m_mode == EditorMode::Select )
	{		
		if( wheel > 0 ) m_currentGridHeight += m_currentGridStepHeight;
		else if( wheel < 0 ) m_currentGridHeight -= m_currentGridStepHeight;
	}
	else if( m_mode == EditorMode::Add )
	{
		if( wheel > 0.0f ) m_collectModels.NextMesh();
		if( wheel < 0.0f ) m_collectModels.PrevMesh();

		if( input.IsMouseButtonPressed(0) )
		{
			addObjectInMap();
		}
	}
}
//-----------------------------------------------------------------------------
void EditorState::OnFrame()
{
	auto& renderSystem = GetRenderSystem();
	auto& graphicsSystem = GetGraphicsSystem();

	if( m_windowWidth != GetWindowWidth() || m_windowHeight != GetWindowHeight() )
	{
		m_windowWidth = GetWindowWidth();
		m_windowHeight = GetWindowHeight();
		m_perspective = glm::perspective(glm::radians(45.0f), GetWindowSizeAspect(), 0.01f, 1000.f);
		renderSystem.SetViewport(m_windowWidth, m_windowHeight);
	}

	renderSystem.ClearFrame();

	if( !m_freeLook && m_mode == EditorMode::Add )
	{
		m_cursors.Draw(renderSystem, GetInput(), m_perspective, m_camera, m_currentGridHeight);
		m_collectModels.DrawPreview(renderSystem, graphicsSystem, m_perspective, m_camera.GetViewMatrix(), m_objPosition);
	}

	m_collectModels.Draw(renderSystem, graphicsSystem, m_perspective, m_camera.GetViewMatrix(), m_map);

	m_gridDrawer.Draw(renderSystem, m_perspective * m_camera.GetViewMatrix(), { 0.0f, m_currentGridHeight, 0.0f }, m_currentSizeCell, m_currentSizeMap);

	drawImgui();
}
//-----------------------------------------------------------------------------
bool EditorState::createImgui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls
	io.IniFilename = nullptr; // не нужно хранить файл настроек

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
bool EditorState::updateImgui()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if( m_camTelData != m_oldCamTelData ) // телепорт камеры слайдом
	{
		m_camera.Teleport(m_camTelData);
		m_oldCamTelData = m_camTelData;
	}

	// переключение между левой и правой панелями редактора
	const ImGuiIO& io = ImGui::GetIO();
	if( !io.WantCaptureMouse && !io.WantCaptureKeyboard )
		return false;

	return true;
}
//-----------------------------------------------------------------------------
void EditorState::drawImgui()
{
	// Main Window
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

		ImGui::SetNextWindowSize(ImVec2((float)GetWindowWidth(), (float)36));
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::Begin("Editor Menu", nullptr, flags);
		ImGui::SetNextItemWidth(100);
		ImGui::Combo("Editor Mode", (int*)&m_selectMode, "Select\0Add\0");
		ImGui::SameLine();
		ImGui::End();
	}

	// Info map
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		ImGui::SetNextWindowSize(ImVec2(150.0f, 200.0f));
		ImGui::SetNextWindowPos(ImVec2(0, 36));
		ImGui::Begin("Grid", nullptr, flags);

		ImGui::SetNextItemWidth(50);
		ImGui::InputFloat("Height", &m_currentGridHeight);
		ImGui::SetNextItemWidth(50);
		ImGui::InputFloat("Step", &m_currentGridStepHeight);
		ImGui::SetNextItemWidth(50);
		ImGui::InputFloat("Step Cell", &m_currentSizeCell);
		ImGui::SetNextItemWidth(50);
		ImGui::InputFloat("Size Map", &m_currentSizeMap);

		ImGui::Text(
			"+/- :add height\n"
			"CTRL+/- :add step\n"
			"Shift+/- :add cell\n"
			"down +and- : reset");

		ImGui::End();
	}


	// Camera Teleport Window
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		const ImS32 s32_zeroXZ = -100, s32_maxXZ = 1100;
		const ImS32 s32_zeroY = -500, s32_maxY = 500;
		ImGui::SetNextWindowSize(ImVec2(270.0f, 100.0f));
		ImGui::SetNextWindowPos(ImVec2(150, 36));
		ImGui::Begin("Camera Teleport", nullptr, flags);
		ImGui::SliderScalar("mouse X", ImGuiDataType_S32, &m_camTelData.x, &s32_zeroXZ, &s32_maxXZ, "%d");
		ImGui::SliderScalar("mouse Y", ImGuiDataType_S32, &m_camTelData.y, &s32_zeroY, &s32_maxY, "%d");
		ImGui::SliderScalar("mouse Z", ImGuiDataType_S32, &m_camTelData.z, &s32_zeroXZ, &s32_maxXZ, "%d");
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
//-----------------------------------------------------------------------------
void EditorState::selectMode()
{
	if( m_selectMode == 0 && m_mode != EditorMode::Select )
	{
		m_mode = EditorMode::Select;
	}
	else if( m_selectMode == 1 && m_mode != EditorMode::Add )
	{
		m_mode = EditorMode::Add;
	}
	else if( GetInput().IsKeyPressed(Input::KEY_TAB) )
	{
		if( m_mode == EditorMode::Select )
		{
			m_mode = EditorMode::Add;
			m_selectMode = 1;
		}
		else
		{
			m_mode = EditorMode::Select;
			m_selectMode = 0;
		}
	}
}
//-----------------------------------------------------------------------------
void EditorState::cameraUpdate(float deltaTime)
{
	Input& input = GetInput();

	if( input.IsMouseButtonDown(1) )
	{
		m_freeLook = true;
		input.SetMouseLock(true);
	}
	else
	{
		if( m_freeLook ) input.SetMouseLock(false);
		m_freeLook = false;
	}

	const float mouseSensitivity = 30.0f * deltaTime;
	float moveSpeed = 16.0f * deltaTime;
	const glm::vec3 oldCameraPos = m_camera.position;

	if( input.IsKeyDown(Input::KEY_LEFT_SHIFT) ) moveSpeed *= 2.0f;

	if( input.IsKeyDown(Input::KEY_W) ) m_camera.MoveBy(moveSpeed);
	if( input.IsKeyDown(Input::KEY_S) ) m_camera.MoveBy(-moveSpeed);
	if( input.IsKeyDown(Input::KEY_A) ) m_camera.StrafeBy(moveSpeed);
	if( input.IsKeyDown(Input::KEY_D) ) m_camera.StrafeBy(-moveSpeed);
	if( input.IsKeyDown(Input::KEY_Q) ) m_camera.RotateLeftRight(-moveSpeed * 12.0f);
	if( input.IsKeyDown(Input::KEY_E) ) m_camera.RotateLeftRight(moveSpeed * 12.0f);

	if( m_freeLook )
	{
		const glm::vec2 delta = input.GetMouseDeltaPosition();
		if( delta.x != 0.0f )  m_camera.RotateLeftRight(delta.x * mouseSensitivity);
		if( delta.y != 0.0f )  m_camera.RotateUpDown(-delta.y * mouseSensitivity);
	}

	m_oldCamTelData = m_camTelData = { m_camera.position.x, m_camera.position.y, m_camera.position.z };
}
//-----------------------------------------------------------------------------
void EditorState::addObjectInMap()
{
	EditorMapObject object;
	object.fileNameModel = m_collectModels.GetFileModelName();
	object.nameMeshInFile = m_collectModels.GetCurrentMeshName();
	object.meshIndex = m_collectModels.GetCurrentMeshId();
	object.position = m_collectModels.GetCurrentPosition();
	object.scale = m_collectModels.GetCurrentScale();
	object.rotation = m_collectModels.GetCurrentRotation();

	m_map.AddObject(object);
}
//-----------------------------------------------------------------------------
void EditorState::modGrid()
{
	Input& input = GetInput();

	if( input.IsKeyDown(Input::KEY_KP_ADD) && input.IsKeyDown(Input::KEY_KP_SUBTRACT) )
	{
		m_currentGridHeight = 0.0f;
		m_currentGridStepHeight = 0.1f;
		m_currentSizeCell = 1.0f;
	}
	else if( input.IsKeyPressed(Input::KEY_KP_ADD) )
	{
		if( input.IsKeyDown(Input::KEY_LEFT_CONTROL) )
			m_currentGridStepHeight += 0.1f;
		else if( input.IsKeyDown(Input::KEY_LEFT_SHIFT) )
			m_currentSizeCell += m_currentGridStepHeight;
		else
			m_currentGridHeight += m_currentGridStepHeight;
	}
	else if( input.IsKeyPressed(Input::KEY_KP_SUBTRACT) )
	{
		if( input.IsKeyDown(Input::KEY_LEFT_CONTROL) )
			m_currentGridStepHeight -= 0.1f;
		else if( input.IsKeyDown(Input::KEY_LEFT_SHIFT) )
			m_currentSizeCell -= m_currentGridStepHeight;
		else
			m_currentGridHeight -= m_currentGridStepHeight;
	}
}
//-----------------------------------------------------------------------------