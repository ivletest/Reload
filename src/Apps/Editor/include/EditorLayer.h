#ifndef _ReloadEditor_EditorLayer_h_
#define _ReloadEditor_EditorLayer_h_

#include <string>
#include <utility>
#include <Magnum/Trade/TextureData.h>
#include <Magnum/ImGuiIntegration/Context.hpp>

#include "Types.h"

#include "Layer.h"
#include "Scene/Scene.h"
#include "SceneHierarchyPanel.h"
#include "ImGui/ImGuizmo.h"

using namespace Magnum;

namespace ReloadEditor
{
	class EditorLayer : Reload::Layer
	{
	public:
		enum class PropertyFlag
		{
			None = 0,
			ColorProperty = 1
		};

	public:
		EditorLayer();
		virtual ~EditorLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate() override;

		virtual void OnViewportEvent(ViewportEvent& event) override;
        virtual void OnMousePress(MouseEvent& event) override;
        virtual void OnMouseRelease(MouseEvent& event) override;
        virtual void OnMouseMove(MouseMoveEvent& event) override;
        virtual void OnMouseScroll(MouseScrollEvent& event) override;
        virtual void OnKeyPress(KeyEvent& event) override;

		virtual void OnImGuiRender() override;
		virtual void OnEvent(InputEvent &e) override;

		Magnum::Containers::Optional<Reload::Scene> &GetScene() { return _scene; }

		// ImGui UI helpers
		bool Property(const std::string &name, bool &value);
		void Property(const std::string &name, float &value, float min = -1.0f, float max = 1.0f, PropertyFlag flags = PropertyFlag::None);
		void Property(const std::string &name, Vector2 &value, PropertyFlag flags);
		void Property(const std::string &name, Vector2 &value, float min = -1.0f, float max = 1.0f, PropertyFlag flags = PropertyFlag::None);
		void Property(const std::string &name, Vector3 &value, PropertyFlag flags);
		void Property(const std::string &name, Vector3 &value, float min = -1.0f, float max = 1.0f, PropertyFlag flags = PropertyFlag::None);
		void Property(const std::string &name, Vector4 &value, PropertyFlag flags);
		void Property(const std::string &name, Vector4 &value, float min = -1.0f, float max = 1.0f, PropertyFlag flags = PropertyFlag::None);

		void ShowBoundingBoxes(bool show, bool onTop = false);

	private:
		std::pair<float, float> GetMouseViewportSpace();
		std::pair<Vector3, Vector3> CastRay(float mx, float my);

	private:
		// Containers::Pointer<SceneHierarchyPanel> m_SceneHierarchyPanel;

		Magnum::Containers::Optional<Reload::Scene> _scene;
		Magnum::Containers::Optional<Reload::Scene> m_SphereScene;
		Magnum::Containers::Optional<Reload::Scene> m_ActiveScene;

		// Entity* m_MeshEntity = nullptr;

		// Ref<Shader> m_BrushShader;
		// Ref<Mesh> m_PlaneMesh;
		// Ref<Material> m_SphereBaseMaterial;

		// Ref<Material> m_MeshMaterial;
		// std::vector<Ref<MaterialInstance>> m_MetalSphereMaterialInstances;
		// std::vector<Ref<MaterialInstance>> m_DielectricSphereMaterialInstances;

		float m_GridScale = 16.025f, m_GridSize = 0.025f;

		struct AlbedoInput
		{
			Vector3 Color = {0.972f, 0.96f, 0.915f}; // Silver, from https://docs.unrealengine.com/en-us/Engine/Rendering/Materials/PhysicallyBased
			// std::shared_ptr<Trade::TextureData::Type::Texture2D> TextureMap;
			bool SRGB = true;
			bool UseTexture = false;
		};
		AlbedoInput m_AlbedoInput;

		struct NormalInput
		{
			// Ref<Texture2D> TextureMap;
			bool UseTexture = false;
		};
		NormalInput m_NormalInput;

		struct MetalnessInput
		{
			float Value = 1.0f;
			// Ref<Texture2D> TextureMap;
			bool UseTexture = false;
		};
		MetalnessInput m_MetalnessInput;

		struct RoughnessInput
		{
			float Value = 0.2f;
			// Ref<Texture2D> TextureMap;
			bool UseTexture = false;
		};
		RoughnessInput m_RoughnessInput;

		// PBR params
		bool m_RadiancePrefilter = false;

		float m_EnvMapRotation = 0.0f;

		enum class SceneType : uint32_t
		{
			Spheres = 0,
			Model = 1
		};
		SceneType m_SceneType;

		// Editor resources
		// Ref<Texture2D> m_CheckerboardTex;

		Vector2 m_ViewportBounds[2];
		int m_GizmoType = -1; // -1 = no gizmo
		float m_SnapValue = 0.5f;
		bool m_AllowViewportCameraEvents = false;
		bool m_DrawOnTopBoundingBoxes = false;

		bool m_UIShowBoundingBoxes = false;
		bool m_UIShowBoundingBoxesOnTop = false;

		struct SelectedSubmesh
		{
			Magnum::MeshPrimitive *Mesh;
			float Distance;
		};

		std::vector<SelectedSubmesh> m_SelectedSubmeshes;
		Magnum::Matrix4 *m_CurrentlySelectedTransform = nullptr;
	};

} // namespace ReloadEditor
#endif