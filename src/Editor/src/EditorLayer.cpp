#include "EditorLayer.h"

#include <Corrade/PluginManager/Manager.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <MagnumPlugins/TgaImporter/TgaImporter.h>
#include <Corrade/Utility/Directory.h>

#include "Editor.h"

using namespace Magnum;

namespace ReloadEditor
{
	const std::string TGA_IMPORTER = "TgaImporter";
	const ImVec2 TEX_THUMB_SIZE = ImVec2(32, 32);

	static void ImGuiShowHelpMarker(const char *desc)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	EditorLayer::EditorLayer() : m_SceneType(SceneType::Model)
	{
		/* Setup the resource manager */
		{
			// Containers::Pointer<Trade::AbstractImporter> imageImporter =
			// 	manager.loadAndInstantiate("TgaImporter");

			// Containers::Pointer<Trade::AbstractImporter> gltfImporter =
			// 	manager.loadAndInstantiate("TinyGltfImporter");

			// CORRADE_INTERNAL_ASSERT(imageImporter);
			// CORRADE_INTERNAL_ASSERT(gltfImporter);

			// _resourceManager.set(
			// 	"tga-importer",
			// 	imageImporter.release(),
			// 	ResourceDataState::Final,
			// 	ResourcePolicy::Manual);

			// _resourceManager.set(
			// 	"tiny-gltf-importer",
			// 	gltfImporter.release(),
			// 	ResourceDataState::Final,
			// 	ResourcePolicy::Manual);
		}
	}

	EditorLayer::~EditorLayer()
	{
	}

	void EditorLayer::OnAttach()
	{
		// ImGui Colors
		ImVec4 *colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.0f); // Window background
		colors[ImGuiCol_ChildBg] = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.5f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.3f, 0.3f, 0.3f, 0.5f); // Widget backgrounds
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.4f, 0.4f, 0.4f, 0.4f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.4f, 0.4f, 0.4f, 0.6f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.0f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.29f, 0.29f, 0.29f, 1.0f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.0f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.0f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.0f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.0f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.51f, 0.51f, 0.51f, 0.7f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.66f, 0.66f, 0.66f, 1.0f);
		colors[ImGuiCol_Button] = ImVec4(0.44f, 0.44f, 0.44f, 0.4f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.46f, 0.47f, 0.48f, 1.0f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.42f, 0.42f, 0.42f, 1.0f);
		colors[ImGuiCol_Header] = ImVec4(0.7f, 0.7f, 0.7f, 0.31f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.7f, 0.7f, 0.7f, 0.8f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.48f, 0.5f, 0.52f, 1.0f);
		colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.5f, 0.5f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.72f, 0.72f, 0.72f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.0f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.91f, 0.91f, 0.91f, 0.25f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.81f, 0.81f, 0.81f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.46f, 0.46f, 0.46f, 0.95f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.0f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.43f, 0.35f, 1.0f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.73f, 0.6f, 0.15f, 1.0f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6f, 0.0f, 1.0f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.87f, 0.87f, 0.87f, 0.35f);
		// colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.8f, 0.8f, 0.8f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.9f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.6f, 0.6f, 1.0f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);

		// auto environment = Environment::Load("assets/env/birchwood_4k.hdr");

		// Model Scene
		{
			m_ActiveScene.emplace(Editor::Get().windowSize(), Editor::Get().framebufferSize());

			// _scene->SetCamera(Camera(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f)));

			// _scene->SetEnvironment(environment);

			// m_MeshEntity = _scene->CreateEntity("Test Entity");

			// auto mesh = CreateRef<Mesh>("assets/meshes/TestScene.fbx");
			// m_MeshEntity->SetMesh(mesh);

			// m_MeshMaterial = mesh->GetMaterial();

			// auto secondEntity = _scene->CreateEntity("Gun Entity");
			// secondEntity->Transform() = glm::translate(glm::mat4(1.0f), { 5, 5, 5 }) * glm::scale(glm::mat4(1.0f), {10, 10, 10});
			// mesh = CreateRef<Mesh>("assets/models/m1911/M1911Materials.fbx");
			// secondEntity->SetMesh(mesh);
		}

		// 	// Sphere Scene
		// 	{
		// 		m_SphereScene = CreateRef<Scene>("PBR Sphere Scene");
		// 		m_SphereScene->SetCamera(Camera(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f)));

		// 		m_SphereScene->SetEnvironment(environment);

		// 		auto sphereMesh = CreateRef<Mesh>("assets/models/Sphere1m.fbx");
		// 		m_SphereBaseMaterial = sphereMesh->GetMaterial();

		// 		float x = -4.0f;
		// 		float roughness = 0.0f;
		// 		for (int i = 0; i < 8; i++)
		// 		{
		// 			auto sphereEntity = m_SphereScene->CreateEntity();

		// 			Ref<MaterialInstance> mi = CreateRef<MaterialInstance>(m_SphereBaseMaterial);
		// 			mi->Set("u_Metalness", 1.0f);
		// 			mi->Set("u_Roughness", roughness);
		// 			x += 1.1f;
		// 			roughness += 0.15f;
		// 			m_MetalSphereMaterialInstances.push_back(mi);

		// 			sphereEntity->SetMesh(sphereMesh);
		// 			sphereEntity->SetMaterial(mi);
		// 			sphereEntity->Transform() = translate(mat4(1.0f), vec3(x, 0.0f, 0.0f));
		// 		}

		// 		x = -4.0f;
		// 		roughness = 0.0f;
		// 		for (int i = 0; i < 8; i++)
		// 		{
		// 			auto sphereEntity = m_SphereScene->CreateEntity();

		// 			Ref<MaterialInstance> mi(new MaterialInstance(m_SphereBaseMaterial));
		// 			mi->Set("u_Metalness", 0.0f);
		// 			mi->Set("u_Roughness", roughness);
		// 			x += 1.1f;
		// 			roughness += 0.15f;
		// 			m_DielectricSphereMaterialInstances.push_back(mi);

		// 			sphereEntity->SetMesh(sphereMesh);
		// 			sphereEntity->SetMaterial(mi);
		// 			sphereEntity->Transform() = translate(mat4(1.0f), vec3(x, 1.2f, 0.0f));
		// 		}
		// 	}

		// 	m_ActiveScene = _scene;
		// 	m_SceneHierarchyPanel = CreateScope<SceneHierarchyPanel>(m_ActiveScene);

		// 	m_PlaneMesh.reset(new Mesh("assets/models/Plane1m.obj"));

		// 	// Editor

		if (!(_pluginManager.load(TGA_IMPORTER) & PluginManager::LoadState::Loaded))
		{
			Utility::Error{}
				<< "The requested plugin" << TGA_IMPORTER << "cannot be loaded.";
		}

		auto checkerBoardImage = Corrade::Utility::Directory::join(
			{Corrade::Utility::Directory::current(), "Assets", "img", "Editor", "Checkerboard.tga"});

		Containers::Pointer<Trade::AbstractImporter> importer =
			_pluginManager.instantiate(TGA_IMPORTER);

		if (!importer->openFile(checkerBoardImage))
		{
			std::exit(1);
		}

		Containers::Optional<Trade::ImageData2D> image = importer->image2D(0);
		CORRADE_INTERNAL_ASSERT(image);
		_checkerboardTex
			.setWrapping(GL::SamplerWrapping::ClampToEdge)
			.setMagnificationFilter(GL::SamplerFilter::Nearest)
			.setMinificationFilter(GL::SamplerFilter::Nearest)
			.setStorage(1, GL::textureFormat(image->format()), image->size())
			.setSubImage(0, {}, *image);

		// 	// Set lights
		// 	auto& light = _scene->GetLight();
		// 	light.Direction = { -0.5f, -0.5f, 1.0f };
		// 	light.Radiance = { 1.0f, 1.0f, 1.0f };

		// 	m_CurrentlySelectedTransform = &m_MeshEntity->Transform();
	}

	void EditorLayer::OnDetach()
	{
	}

	void EditorLayer::OnUpdate()
	{
		// THINGS TO LOOK AT:
		// - BRDF LUT
		// - Tonemapping and proper HDR pipeline
		// using namespace Hazel;
		// using namespace glm;

		// m_MeshMaterial->Set("u_AlbedoColor", m_AlbedoInput.Color);
		// m_MeshMaterial->Set("u_Metalness", m_MetalnessInput.Value);
		// m_MeshMaterial->Set("u_Roughness", m_RoughnessInput.Value);
		// m_MeshMaterial->Set("u_AlbedoTexToggle", m_AlbedoInput.UseTexture ? 1.0f : 0.0f);
		// m_MeshMaterial->Set("u_NormalTexToggle", m_NormalInput.UseTexture ? 1.0f : 0.0f);
		// m_MeshMaterial->Set("u_MetalnessTexToggle", m_MetalnessInput.UseTexture ? 1.0f : 0.0f);
		// m_MeshMaterial->Set("u_RoughnessTexToggle", m_RoughnessInput.UseTexture ? 1.0f : 0.0f);
		// m_MeshMaterial->Set("u_EnvMapRotation", m_EnvMapRotation);

		// m_SphereBaseMaterial->Set("u_AlbedoColor", m_AlbedoInput.Color);
		// m_SphereBaseMaterial->Set("lights", _scene->GetLight());
		// m_SphereBaseMaterial->Set("u_RadiancePrefilter", m_RadiancePrefilter ? 1.0f : 0.0f);
		// m_SphereBaseMaterial->Set("u_AlbedoTexToggle", m_AlbedoInput.UseTexture ? 1.0f : 0.0f);
		// m_SphereBaseMaterial->Set("u_NormalTexToggle", m_NormalInput.UseTexture ? 1.0f : 0.0f);
		// m_SphereBaseMaterial->Set("u_MetalnessTexToggle", m_MetalnessInput.UseTexture ? 1.0f : 0.0f);
		// m_SphereBaseMaterial->Set("u_RoughnessTexToggle", m_RoughnessInput.UseTexture ? 1.0f : 0.0f);
		// m_SphereBaseMaterial->Set("u_EnvMapRotation", m_EnvMapRotation);

		// if (m_AlbedoInput.TextureMap)
		// 	m_MeshMaterial->Set("u_AlbedoTexture", m_AlbedoInput.TextureMap);
		// if (m_NormalInput.TextureMap)
		// 	m_MeshMaterial->Set("u_NormalTexture", m_NormalInput.TextureMap);
		// if (m_MetalnessInput.TextureMap)
		// 	m_MeshMaterial->Set("u_MetalnessTexture", m_MetalnessInput.TextureMap);
		// if (m_RoughnessInput.TextureMap)
		// 	m_MeshMaterial->Set("u_RoughnessTexture", m_RoughnessInput.TextureMap);

		// if (m_AllowViewportCameraEvents)
		// 	_scene->GetCamera().OnUpdate(ts);

		// m_ActiveScene->OnUpdate(ts);

		// if (m_DrawOnTopBoundingBoxes)
		// {
		// 	Hazel::Renderer::BeginRenderPass(Hazel::SceneRenderer::GetFinalRenderPass(), false);
		// 	auto viewProj = _scene->GetCamera().GetViewProjection();
		// 	Hazel::Renderer2D::BeginScene(viewProj, false);
		// 	Renderer::DrawAABB(m_MeshEntity->GetMesh(), m_MeshEntity->Transform());
		// 	Hazel::Renderer2D::EndScene();
		// 	Hazel::Renderer::EndRenderPass();
		// }

		// if (m_SelectedSubmeshes.size())
		// {
		// 	Hazel::Renderer::BeginRenderPass(Hazel::SceneRenderer::GetFinalRenderPass(), false);
		// 	auto viewProj = _scene->GetCamera().GetViewProjection();
		// 	Hazel::Renderer2D::BeginScene(viewProj, false);
		// 	auto& submesh = m_SelectedSubmeshes[0];
		// 	Renderer::DrawAABB(submesh.Mesh->BoundingBox, m_MeshEntity->GetTransform() * submesh.Mesh->Transform);
		// 	Hazel::Renderer2D::EndScene();
		// 	Hazel::Renderer::EndRenderPass();
		// }
	}

	bool EditorLayer::Property(const std::string &name, bool &value)
	{
		ImGui::Text(name.c_str());
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);

		std::string id = "##" + name;
		bool result = ImGui::Checkbox(id.c_str(), &value);

		ImGui::PopItemWidth();
		ImGui::NextColumn();

		return result;
	}

	void EditorLayer::Property(const std::string &name, float &value, float min, float max, EditorLayer::PropertyFlag flags)
	{
		ImGui::Text(name.c_str());
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);

		std::string id = "##" + name;
		ImGui::SliderFloat(id.c_str(), &value, min, max);

		ImGui::PopItemWidth();
		ImGui::NextColumn();
	}

	void EditorLayer::Property(const std::string &name, Vector2 &value, EditorLayer::PropertyFlag flags)
	{
		Property(name, value, -1.0f, 1.0f, flags);
	}

	void EditorLayer::Property(const std::string &name, Vector2 &value, float min, float max, EditorLayer::PropertyFlag flags)
	{
		ImGui::Text(name.c_str());
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);

		std::string id = "##" + name;
		ImGui::SliderFloat2(id.c_str(), value.data(), min, max);

		ImGui::PopItemWidth();
		ImGui::NextColumn();
	}

	void EditorLayer::Property(const std::string &name, Vector3 &value, EditorLayer::PropertyFlag flags)
	{
		Property(name, value, -1.0f, 1.0f, flags);
	}

	void EditorLayer::Property(const std::string &name, Vector3 &value, float min, float max, EditorLayer::PropertyFlag flags)
	{
		ImGui::Text(name.c_str());
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);

		std::string id = "##" + name;
		if ((int)flags & (int)PropertyFlag::ColorProperty)
		{
			ImGui::ColorEdit3(id.c_str(), value.data(), ImGuiColorEditFlags_NoInputs);
		}
		else
		{
			ImGui::SliderFloat3(id.c_str(), value.data(), min, max);
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();
	}

	void EditorLayer::Property(const std::string &name, Vector4 &value, EditorLayer::PropertyFlag flags)
	{
		Property(name, value, -1.0f, 1.0f, flags);
	}

	void EditorLayer::Property(const std::string &name, Vector4 &value, float min, float max, EditorLayer::PropertyFlag flags)
	{

		ImGui::Text(name.c_str());
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);

		std::string id = "##" + name;
		if ((int)flags & (int)PropertyFlag::ColorProperty)
		{
			ImGui::ColorEdit4(id.c_str(), value.data(), ImGuiColorEditFlags_NoInputs);
		}
		else
		{
			ImGui::SliderFloat4(id.c_str(), value.data(), min, max);
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();
	}

	void EditorLayer::ShowBoundingBoxes(bool show, bool onTop)
	{
		// SceneRenderer::GetOptions().ShowBoundingBoxes = show && !onTop;
		// m_DrawOnTopBoundingBoxes = show && onTop;
	}

	void EditorLayer::OnImGuiRender()
	{
		static bool p_open = true;

		static bool opt_fullscreen_persistant = true;
		static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_None;
		bool opt_fullscreen = opt_fullscreen_persistant;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport *viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruDockspace, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		// if (opt_flags & ImGuiDockNodeFlags_PassthruDockspace)
		// {
		// 	window_flags |= ImGuiWindowFlags_NoBackground;
		// }

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace", &p_open, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Dockspace
		ImGuiIO &io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);
		}

		// // Editor Panel ------------------------------------------------------------------------------
		ImGui::Begin("Model");
		if (ImGui::RadioButton("Spheres", (int *)&m_SceneType, (int)SceneType::Spheres))
		{
			// m_ActiveScene = m_SphereScene;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Model", (int *)&m_SceneType, (int)SceneType::Model))
		{
			// _activeScene.emplace(_resourceManager, Editor::Get().windowSize(), Editor::Get().framebufferSize());
		}
		ImGui::End(); // Model

		ImGui::Begin("Environment");

		if (ImGui::Button("Load Environment Map"))
		{
			// std::string filename = Application::Get().OpenFile("*.hdr");
			// if (filename != "")
			// m_ActiveScene->SetEnvironment(Environment::Load(filename));
		}

		// ImGui::SliderFloat("Skybox LOD", &_scene->GetSkyboxLod(), 0.0f, 11.0f);

		ImGui::Columns(2);
		ImGui::AlignTextToFramePadding();

		// auto& light = _scene->GetLight();
		// Property("Light Direction", light.Direction);
		// Property("Light Radiance", light.Radiance, PropertyFlag::ColorProperty);
		// Property("Light Multiplier", light.Multiplier, 0.0f, 5.0f);
		// Property("Exposure", m_ActiveScene->GetCamera().GetExposure(), 0.0f, 5.0f);

		Property("Radiance Prefiltering", m_RadiancePrefilter);
		Property("Env Map Rotation", m_EnvMapRotation, -360.0f, 360.0f);

		if (Property("Show Bounding Boxes", m_UIShowBoundingBoxes))
		{
			ShowBoundingBoxes(m_UIShowBoundingBoxes, m_UIShowBoundingBoxesOnTop);
		}
		if (m_UIShowBoundingBoxes && Property("On Top", m_UIShowBoundingBoxesOnTop))
		{
			ShowBoundingBoxes(m_UIShowBoundingBoxes, m_UIShowBoundingBoxesOnTop);
		}

		ImGui::Columns(1);

		ImGui::Separator();
		{
			ImGui::Text("Mesh");
			// auto mesh = m_MeshEntity->GetMesh();
			// std::string fullpath = mesh ? mesh->GetFilePath() : "None";
			// size_t found = fullpath.find_last_of("/\\");
			// std::string path = found != std::string::npos ? fullpath.substr(found + 1) : fullpath;
			// ImGui::Text(path.c_str()); ImGui::SameLine();
			// if (ImGui::Button("...##Mesh"))
			// {
			// 	std::string filename = Application::Get().OpenFile("");
			// 	if (filename != "")
			// 	{
			// 		auto newMesh = CreateRef<Mesh>(filename);
			// 		// m_MeshMaterial.reset(new MaterialInstance(newMesh->GetMaterial()));
			// 		// m_MeshEntity->SetMaterial(m_MeshMaterial);
			// 		m_MeshEntity->SetMesh(newMesh);
			// 	}
			// }
		}
		ImGui::Separator();

		// // Textures ------------------------------------------------------------------------------
		{
			// Albedo
			if (ImGui::CollapsingHeader("Albedo", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));

				ImGui::Image(
					m_AlbedoInput.TextureMap
						? reinterpret_cast<ImTextureID>(&m_AlbedoInput.TextureMap)
						: reinterpret_cast<ImTextureID>(&_checkerboardTex),
					TEX_THUMB_SIZE);

				ImGui::PopStyleVar();
				if (ImGui::IsItemHovered())
				{
					if (m_AlbedoInput.TextureMap)
					{
						ImGui::BeginTooltip();
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
						// ImGui::TextUnformatted(m_AlbedoInput.TextureMap->GetPath().c_str());
						ImGui::PopTextWrapPos();
						ImGui::Image(reinterpret_cast<ImTextureID>(&m_AlbedoInput.TextureMap), ImVec2(384, 384));
						ImGui::EndTooltip();
					}
					if (ImGui::IsItemClicked())
					{
						// std::string filename = Editor::Get().OpenFile("");
						// if (filename != "")
						// {
						// 	_albedoInput.TextureMap = Texture2D::Create(filename, m_AlbedoInput.SRGB);
						// }
					}
				}
				ImGui::SameLine();
				ImGui::BeginGroup();
				ImGui::Checkbox("Use##AlbedoMap", &m_AlbedoInput.UseTexture);
				if (ImGui::Checkbox("sRGB##AlbedoMap", &m_AlbedoInput.SRGB))
				{
					if (m_AlbedoInput.TextureMap)
					{
						// _albedoInput.TextureMap = Texture2D::Create(_albedoInput.TextureMap->GetPath(), _albedoInput.SRGB);
					}
				}
				ImGui::EndGroup();
				ImGui::SameLine();
				ImGui::ColorEdit3("Color##Albedo", m_AlbedoInput.Color.data(), ImGuiColorEditFlags_NoInputs);
			}
		}
		{
			// Normals
			if (ImGui::CollapsingHeader("Normals", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
				ImGui::Image(m_NormalInput.TextureMap
								 ? reinterpret_cast<ImTextureID>(&m_NormalInput.TextureMap)
								 : reinterpret_cast<ImTextureID>(&_checkerboardTex),
							 TEX_THUMB_SIZE);
				ImGui::PopStyleVar();
				if (ImGui::IsItemHovered())
				{
					if (m_NormalInput.TextureMap)
					{
						ImGui::BeginTooltip();
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
						ImGui::TextUnformatted(m_NormalInput.TextureMap->label().c_str());
						ImGui::PopTextWrapPos();
						ImGui::Image(reinterpret_cast<ImTextureID>(&m_NormalInput.TextureMap), ImVec2(384, 384));
						ImGui::EndTooltip();
					}
					if (ImGui::IsItemClicked())
					{
						// std::string filename = Editor::Get().OpenFile("");
						// if (filename != "")
						// 	_normalInput.TextureMap = Texture2D::Create(filename);
					}
				}
				ImGui::SameLine();
				ImGui::Checkbox("Use##NormalMap", &m_NormalInput.UseTexture);
			}
		}
		{
			// Metalness
			if (ImGui::CollapsingHeader("Metalness", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
				ImGui::Image(
					m_MetalnessInput.TextureMap
						? reinterpret_cast<ImTextureID>(&m_MetalnessInput.TextureMap)
						: reinterpret_cast<ImTextureID>(&_checkerboardTex),
					TEX_THUMB_SIZE);
				ImGui::PopStyleVar();
				if (ImGui::IsItemHovered())
				{
					if (m_MetalnessInput.TextureMap)
					{
						ImGui::BeginTooltip();
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
						ImGui::TextUnformatted(m_MetalnessInput.TextureMap->label().c_str());
						ImGui::PopTextWrapPos();
						ImGui::Image(reinterpret_cast<ImTextureID>(&m_MetalnessInput.TextureMap), ImVec2(384, 384));
						ImGui::EndTooltip();
					}
					if (ImGui::IsItemClicked())
					{
						// std::string filename = Application::Get().OpenFile("");
						// if (filename != "")
						// 	m_MetalnessInput.TextureMap = Texture2D::Create(filename);
					}
				}
				ImGui::SameLine();
				ImGui::Checkbox("Use##MetalnessMap", &m_MetalnessInput.UseTexture);
				ImGui::SameLine();
				ImGui::SliderFloat("Value##MetalnessInput", &m_MetalnessInput.Value, 0.0f, 1.0f);
			}
		}
		{
			// Roughness
			if (ImGui::CollapsingHeader("Roughness", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
				ImGui::Image(
					m_RoughnessInput.TextureMap
						? reinterpret_cast<ImTextureID>(&m_RoughnessInput.TextureMap)
						: reinterpret_cast<ImTextureID>(&_checkerboardTex),
					TEX_THUMB_SIZE);
				ImGui::PopStyleVar();
				if (ImGui::IsItemHovered())
				{
					if (m_RoughnessInput.TextureMap)
					{
						ImGui::BeginTooltip();
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
						ImGui::TextUnformatted(m_RoughnessInput.TextureMap->label().c_str());
						ImGui::PopTextWrapPos();
						ImGui::Image(reinterpret_cast<ImTextureID>(&m_RoughnessInput.TextureMap), ImVec2(384, 384));
						ImGui::EndTooltip();
					}
					if (ImGui::IsItemClicked())
					{
						// std::string filename = Application::Get().OpenFile("");
						// if (filename != "")
						// 	m_RoughnessInput.TextureMap = Texture2D::Create(filename);
					}
				}
				ImGui::SameLine();
				ImGui::Checkbox("Use##RoughnessMap", &m_RoughnessInput.UseTexture);
				ImGui::SameLine();
				ImGui::SliderFloat("Value##RoughnessInput", &m_RoughnessInput.Value, 0.0f, 1.0f);
			}
		}
	
		ImGui::Separator();

		if (ImGui::TreeNode("Shaders"))
		{
			Containers::Array<Shaders::Phong> shaders;
			for (auto& shader : shaders)
			{
				if (ImGui::TreeNode(shader.label().c_str()))
				{
					std::string buttonName = "Reload##" + shader.label();
					if (ImGui::Button(buttonName.c_str()))
					{
						// shader->Reload();
					}
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}

		ImGui::End(); // Environment

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Viewport");

		auto viewportOffset = ImGui::GetCursorPos(); // includes tab bar
		auto viewportSize = ImGui::GetContentRegionAvail();
		// SceneRenderer::SetViewportSize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
		// m_ActiveScene->GetCamera().SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), viewportSize.x, viewportSize.y, 0.1f, 10000.0f));
		// m_ActiveScene->GetCamera().SetViewportSize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
		// ImGui::Image((void*)SceneRenderer::GetFinalColorBufferRendererID(), viewportSize, { 0, 1 }, { 1, 0 });

		static int counter = 0;
		auto windowSize = ImGui::GetWindowSize();
		ImVec2 minBound = ImGui::GetWindowPos();
		minBound.x += viewportOffset.x;
		minBound.y += viewportOffset.y;

		ImVec2 maxBound = {minBound.x + windowSize.x, minBound.y + windowSize.y};
		_viewportBounds[0] = {minBound.x, minBound.y};
		_viewportBounds[1] = {maxBound.x, maxBound.y};
		m_AllowViewportCameraEvents = ImGui::IsMouseHoveringRect(minBound, maxBound);

		// Gizmos
		if (m_GizmoType != -1 && m_CurrentlySelectedTransform)
		{
			float rw = (float)ImGui::GetWindowWidth();
			float rh = (float)ImGui::GetWindowHeight();
			// ImGuizmo::SetOrthographic(false);
			// ImGuizmo::SetDrawlist();
			// ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, rw, rh);

			// bool snap = Input::IsKeyPressed(HZ_KEY_LEFT_CONTROL);
			// ImGuizmo::Manipulate(glm::value_ptr(m_ActiveScene->GetCamera().GetViewMatrix() * m_MeshEntity->Transform()),
			// 	glm::value_ptr(m_ActiveScene->GetCamera().GetProjectionMatrix()),
			// 	(ImGuizmo::OPERATION)m_GizmoType,
			// 	ImGuizmo::LOCAL,
			// 	glm::value_ptr(*m_CurrentlySelectedTransform),
			// 	nullptr,
			// 	snap ? &m_SnapValue : nullptr);
		}

		ImGui::End();
		ImGui::PopStyleVar();

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Docking"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

				if (ImGui::MenuItem("Flag: NoSplit", "", (opt_flags & ImGuiDockNodeFlags_NoSplit) != 0))
					opt_flags ^= ImGuiDockNodeFlags_NoSplit;
				if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (opt_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))
					opt_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
				if (ImGui::MenuItem("Flag: NoResize", "", (opt_flags & ImGuiDockNodeFlags_NoResize) != 0))
					opt_flags ^= ImGuiDockNodeFlags_NoResize;
				//if (ImGui::MenuItem("Flag: PassthruDockspace", "", (opt_flags & ImGuiDockNodeFlags_PassthruDockspace) != 0))       opt_flags ^= ImGuiDockNodeFlags_PassthruDockspace;
				if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (opt_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))
					opt_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
				ImGui::Separator();
				if (ImGui::MenuItem("Close DockSpace", NULL, false, p_open != NULL))
					p_open = false;
				ImGui::EndMenu();
			}
			ImGuiShowHelpMarker(
				"You can _always_ dock _any_ window into another by holding the SHIFT key while moving a window. Try it now!"
				"\n"
				"This demo app has nothing to do with it!"
				"\n\n"
				"This demo app only demonstrate the use of ImGui::DockSpace() which allows you to manually create a docking node _within_ another window. This is useful so you can decorate your main application window (e.g. with a menu bar)."
				"\n\n"
				"ImGui::DockSpace() comes with one hard constraint: it needs to be submitted _before_ any window which may be docked into it. Therefore, if you use a dock spot as the central point of your application, you'll probably want it to be part of the very first window you are submitting to imgui every frame."
				"\n\n"
				"(NB: because of this constraint, the implicit \"Debug\" window can not be docked into an explicit DockSpace() node, because that window is submitted as part of the NewFrame() call. An easy workaround is that you can create your own implicit \"Debug##2\" window after calling DockSpace() and leave it in the window stack for anyone to use.)");

			ImGui::EndMenuBar();
		}

		// m_SceneHierarchyPanel->OnImGuiRender();

		ImGui::End(); // Dockspace
	}

	void EditorLayer::OnEvent(InputEvent &e)
	{
		if (m_AllowViewportCameraEvents)
		{
			// _scene->GetCamera().OnEvent(e);
		}

		// EventDispatcher dispatcher(e);
		// dispatcher.Dispatch<KeyPressedEvent>(HZ_BIND_EVENT_FN(EditorLayer::OnKeyPressedEvent));
		// dispatcher.Dispatch<MouseButtonPressedEvent>(HZ_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
	}

	std::pair<float, float> EditorLayer::GetMouseViewportSpace()
	{
		// auto [mx, my] = ImGui::GetMousePos(); // Input::GetMousePosition();
		// mx -= m_ViewportBounds[0].x;
		// my -= m_ViewportBounds[0].y;
		// auto viewportWidth = m_ViewportBounds[1].x - m_ViewportBounds[0].x;
		// auto viewportHeight = m_ViewportBounds[1].y - m_ViewportBounds[0].y;

		// return
		// {
		//     (mx / viewportWidth) * 2.0f - 1.0f,
		//     ((my / viewportHeight) * 2.0f - 1.0f) * -1.0f
		// };
		return {0.0f, 0.0f};
	}

	std::pair<Vector3, Vector3> EditorLayer::CastRay(float mx, float my)
	{
		Vector4 mouseClipPos = {mx, my, -1.0f, 1.0f};

		// The function in magnum isMatrix.Inverted()
		// auto inverseProj = glm::inverse(m_Scene->GetCamera().GetProjectionMatrix());
		// auto inverseView = glm::inverse(glm::mat3(m_Scene->GetCamera().GetViewMatrix()));

		// glm::vec4 ray = inverseProj * mouseClipPos;
		// glm::vec3 rayPos = m_Scene->GetCamera().GetPosition();
		// glm::vec3 rayDir = inverseView * glm::vec3(ray);

		// return { rayPos, rayDir };
		return {Vector3{0}, Vector3{0}};
	}

	void EditorLayer::OnViewportEvent(ViewportEvent &event)
	{
		m_ActiveScene->OnViewportEvent(event);
	}

	void EditorLayer::OnMousePress(MouseEvent &event)
	{
		m_ActiveScene->OnMousePress(event);
		Vector2i pos = event.position();
		if (event.button() == MouseEvent::Button::Left && event.modifiers() & MouseEvent::Modifier::Alt
			// && !ImGuizmo::IsOver()
		)
		{
			auto [mouseX, mouseY] = GetMouseViewportSpace();
			if (mouseX > -1.0f && mouseX < 1.0f && mouseY > -1.0f && mouseY < 1.0f)
			{
				auto [origin, direction] = CastRay(mouseX, mouseY);

				m_SelectedSubmeshes.clear();
				// auto mesh = m_MeshEntity->GetMesh();
				// auto& submeshes = mesh->GetSubmeshes();
				float lastT = std::numeric_limits<float>::max();
				// for (uint32_t i = 0; i < submeshes.size(); i++)
				// {
				// 	auto& submesh = submeshes[i];
				// 	Ray ray = {
				// 		glm::inverse(m_MeshEntity->GetTransform() * submesh.Transform) * glm::vec4(origin, 1.0f),
				// 		glm::inverse(glm::mat3(m_MeshEntity->GetTransform()) * glm::mat3(submesh.Transform)) * direction
				// 	};

				// 	float t;
				// 	bool intersects = ray.IntersectsAABB(submesh.BoundingBox, t);
				// 	if (intersects)
				// 	{
				// 		const auto& triangleCache = mesh->GetTriangleCache(i);
				// 		for (const auto& triangle : triangleCache)
				// 		{
				// 			if (ray.IntersectsTriangle(triangle.V0.Position, triangle.V1.Position, triangle.V2.Position, t))
				// 			{
				// 				HZ_WARN("INTERSECTION: {0}, t={1}", submesh.NodeName, t);
				// 				m_SelectedSubmeshes.push_back({ &submesh, t });
				// 				break;
				// 			}
				// 		}
				// 	}
				// }
				std::sort(m_SelectedSubmeshes.begin(), m_SelectedSubmeshes.end(), [](auto &a, auto &b) { return a.Distance < b.Distance; });

				// TODO: Handle mesh being deleted, etc.
				if (m_SelectedSubmeshes.size())
				{
					// m_CurrentlySelectedTransform = &m_SelectedSubmeshes[0].Mesh->Transform;
				}
				else
				{
					// m_CurrentlySelectedTransform = &m_MeshEntity->Transform();
				}
			}
		}
	}

	void EditorLayer::OnMouseRelease(MouseEvent &event)
	{
		m_ActiveScene->OnMouseRelease(event);
	}

	void EditorLayer::OnMouseMove(MouseMoveEvent &event)
	{
		m_ActiveScene->OnMouseMove(event);
	}

	void EditorLayer::OnMouseScroll(MouseScrollEvent &event)
	{
		m_ActiveScene->OnMouseScroll(event);
	}

	void EditorLayer::OnKeyPress(KeyEvent &event)
	{
		switch (event.key())
		{
		case KeyEvent::Key::Q:
			m_GizmoType = -1;
			break;
		case KeyEvent::Key::W:
			// m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;
		case KeyEvent::Key::E:
			// m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			break;
		case KeyEvent::Key::R:
			// m_GizmoType = ImGuizmo::OPERATION::SCALE;
			break;
		case KeyEvent::Key::G:
			// Toggle grid
			if (event.modifiers() & KeyEvent::Modifier::Ctrl)
			{
				// SceneRenderer::GetOptions().ShowGrid = !SceneRenderer::GetOptions().ShowGrid;
			}
			break;
		case KeyEvent::Key::B:
			// Toggle bounding boxes
			if (event.modifiers() & KeyEvent::Modifier::Ctrl)
			{
				m_UIShowBoundingBoxes = !m_UIShowBoundingBoxes;
				ShowBoundingBoxes(m_UIShowBoundingBoxes, m_UIShowBoundingBoxesOnTop);
			}
			break;
		}

		m_ActiveScene->OnKeyPress(event);
	}

	void EditorLayer::SetThumbnail(Magnum::Containers::Optional<Magnum::GL::Texture2D> texture)
	{
		ImGui::Image(&texture
						 ? reinterpret_cast<ImTextureID>(&texture)
						 : reinterpret_cast<ImTextureID>(&_checkerboardTex),
					 TEX_THUMB_SIZE);
	}

} // namespace ReloadEditor