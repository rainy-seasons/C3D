#include "C3D_UI.h"

UI::UI(GLFWwindow* window, UIParams& params)
	: m_window(window), m_uiParams(params) 
{ }

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
			if (m_uiParams.callback)
			{
				m_uiParams.callback(filePathName);
			}
		}
		// Close the dialog
		ImGuiFileDialog::Instance()->Close();
	}
	ImGui::Combo("Render Mode", &m_uiParams.renderMode, "Depth (Textureless)\0Depth (Textured)\0Direct Light\0Point Light\0Spot Light\0");
	ImGui::Combo("Polygon Mode", &m_uiParams.polygonMode, "Fill\0Line\0Point\0");
	ImGui::Checkbox("Draw Grass", &m_uiParams.drawGrass);
	ImGui::DragFloat3("Rotation (x, y, z)", m_uiParams.modelRotation, 1.0f);
	ImGui::Checkbox("Show Normals", &m_uiParams.drawNormals);
	ImGui::InputFloat("Normal Length", &m_uiParams.normalLength, 0.01f, 0.1f);
	ImGui::ColorEdit3("Normals Color", m_uiParams.normalsColor);
	ImGui::Checkbox("Show Light Positions", &m_uiParams.showLightPositions);

	if (ImGui::Button("Add Light"))
	{
		m_uiParams.lightsVec->push_back(Light{
			glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), // Color
			glm::vec3(10.0f, 5.0f, 20.0f),     // Pos
			glm::mat4(1.0f)					   // Model matrix
		});
	}

	int i = 0;
	while (i < m_uiParams.lightsVec->size())
	{
		std::string lightLabel = "Light " + std::to_string(i + 1);

		if (ImGui::TreeNode(lightLabel.c_str())) 
		{
			ImGui::ColorEdit4("Color", glm::value_ptr((*m_uiParams.lightsVec)[i].Color));
			ImGui::DragFloat3("Position", glm::value_ptr((*m_uiParams.lightsVec)[i].Pos)); 
			if (ImGui::Button(("Delete##" + std::to_string(i)).c_str()) && m_uiParams.lightsVec->size() > 1)
			{
				m_uiParams.lightsVec->erase(m_uiParams.lightsVec->begin() + i);
				ImGui::TreePop();
				continue; // Skip i++ so we don't skip the next element
			}
			ImGui::TreePop();
		}
		i++;
	}

	ImGui::End();
}

void UI::Shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
