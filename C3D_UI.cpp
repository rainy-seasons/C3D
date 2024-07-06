#include "C3D_UI.h"

UI::UI(const UIParams& params)
	: m_window(params.window),
	m_renderMode(&params.renderMode),
	m_polygonMode(&params.polygonMode),
	m_drawGrass(&params.drawGrass),
	m_drawNormals(&params.drawNormals),
	m_normalLen(&params.normalLength),
	m_callback(params.callback)
{
	// Copy array pointers
	for (int i = 0; i < 4; ++i)
	{
		m_normalsColor[i] = &params.normalsColor[i];
	}

	for (int i = 0; i < 3; ++i)
	{
		m_modelRotation[i] = &params.modelRotation[i];
	}
}

UI::~UI()
{
	Shutdown();
}

void UI::Setup()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void UI::Render()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	DrawImgui();
	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::DrawImgui()
{
	ImGui::Begin("C3D");

	// For loading models at runtime
	if (ImGui::Button("Open File"))
	{
		IGFD::FileDialogConfig config;
		config.path = ".";
		ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".gltf", config);
	}

	// Always draw the file dialog
	if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
			std::cout << "Importing file: " << filePathName << "..." << std::endl;
			if (m_callback)
			{
				m_callback(filePathName);
			}
		}
		// Close the dialog
		ImGuiFileDialog::Instance()->Close();
	}
	ImGui::Combo("Render Mode", m_renderMode, "Depth (Textureless)\0Depth (Textured)\0Direct Light\0Point Light\0Spot Light\0");
	ImGui::Combo("Polygon Mode", m_polygonMode, "Fill\0Line\0Point\0");
	ImGui::Checkbox("Draw Grass", m_drawGrass);
	ImGui::DragFloat3("Rotation (x, y, z)", *m_modelRotation, 1.0f);
	ImGui::Checkbox("Show Normals", m_drawNormals);
	ImGui::InputFloat("Normal Length", m_normalLen, 0.01f, 0.1f);
	ImGui::ColorEdit3("Normals Color", *m_normalsColor);

	ImGui::End();
}

void UI::Shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
