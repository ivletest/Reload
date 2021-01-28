// #ifndef _ReloadEditor_SceneHierarchyPanel_h_
// #define _ReloadEditor_SceneHierarchyPanel_h_

// #include <Magnum/Mesh.h>
// #include "Scene.h"

// namespace ReloadEditor
// {
// 	class SceneHierarchyPanel
// 	{
// 	public:
// 		SceneHierarchyPanel(const Ref<Reload::Scene> &scene);

// 		void SetContext(const Ref<Reload::Scene> &scene);

// 		void OnImGuiRender();

// 	private:
// 		void DrawEntityNode(Entity *entity, uint32_t &imguiEntityID, uint32_t &imguiMeshID);
// 		void DrawMeshNode(const Ref<Magnum::MeshPrimitive> &mesh, uint32_t &imguiMeshID);
// 		void MeshNodeHierarchy(const Ref<Magnum::MeshPrimitive> &mesh, aiNode *node, const Matrix4 &parentTransform = Matrix4(1.0f), uint32_t level = 0);

// 	private:
// 		Ref<Reload::Scene> m_Context;
// 		Ref<Magnum::MeshPrimitive> m_SelectionContext;
// 	};
// } // namespace ReloadEditor
// #endif