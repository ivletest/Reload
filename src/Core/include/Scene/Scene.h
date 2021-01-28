#ifndef _Reload_Scene_h_
#define _Reload_Scene_h_

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/DebugStl.h>
#include <Magnum/ImageView.h>
#include <Magnum/Mesh.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/Math/Color.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Shaders/DistanceFieldVector.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/Trade/MeshData.h>
#include <Magnum/Trade/MeshObjectData3D.h>
#include <Magnum/Trade/PhongMaterialData.h>
#include <Magnum/Trade/SceneData.h>
#include <Magnum/Trade/TextureData.h>

#include "Types.h"
#include "Entities/CameraRig.h"

namespace Reload
{
    class CameraRig;
    
    class Scene
    {
    public:
        explicit Scene(
            const Magnum::Vector2i windowSize, 
            const Magnum::Vector2i frameBufferSize);
        ~Scene() = default;

        void OnDrawEvent(bool &camChanged);
        void OnViewportEvent(ViewportEvent& event);
        void OnMousePress(MouseEvent& event);
        void OnMouseRelease(MouseEvent& event);
        void OnMouseMove(MouseMoveEvent& event);
        void OnMouseScroll(MouseScrollEvent& event);
        void OnKeyPress(KeyEvent& event);

        Object3D CreateObject(const std::string& name = "");

    private:
        void addObject(
            Magnum::Trade::AbstractImporter& importer, 
            Magnum::Containers::ArrayView<const Magnum::Containers::Optional<Magnum::Trade::PhongMaterialData>> materials, 
            Object3D& parent, 
            Magnum::UnsignedInt i);

        ReloadResourceManager _resourceManager;
        Magnum::Shaders::Phong _coloredShader;
        Magnum::Shaders::Phong _texturedShader{Magnum::Shaders::Phong::Flag::DiffuseTexture};
        Magnum::Containers::Array<Magnum::Containers::Optional<Magnum::GL::Mesh>> _meshes;
        Magnum::Containers::Array<Magnum::Containers::Optional<Magnum::GL::Texture2D>> _textures;

        Scene3D _scene;
        Magnum::SceneGraph::DrawableGroup3D _drawables;
        Magnum::Containers::Optional<CameraRig> _cameraRig {};
    };

    //------------------------------------------------------------------------------
    // Drawables
    //------------------------------------------------------------------------------

    class ColoredDrawable : public Magnum::SceneGraph::Drawable3D
    {
    public:
        explicit ColoredDrawable(
            Object3D &object, 
            Magnum::Shaders::Phong &shader, 
            Magnum::GL::Mesh &mesh, 
            const Magnum::Color4 &color,
            Magnum::SceneGraph::DrawableGroup3D &group)
            : Magnum::SceneGraph::Drawable3D{object, &group}, 
              _shader(shader), 
              _mesh(mesh), 
              _color{color} {}

    private:
        void draw(
            const Magnum::Matrix4 &transformationMatrix, 
            Magnum::SceneGraph::Camera3D &camera) override;

        Magnum::Shaders::Phong &_shader;
        Magnum::GL::Mesh &_mesh;
        Magnum::Color4 _color;
    };

    class TexturedDrawable : public Magnum::SceneGraph::Drawable3D
    {
    public:
        explicit TexturedDrawable(
            Object3D &object,
            Magnum::Shaders::Phong &shader, 
            Magnum::GL::Mesh &mesh, 
            Magnum::GL::Texture2D &texture, 
            Magnum::SceneGraph::DrawableGroup3D &group)
            : Magnum::SceneGraph::Drawable3D{object, &group}, 
              _shader(shader), 
              _mesh(mesh), 
              _texture(texture) {}

    private:
        void draw(
            const Magnum::Matrix4 &transformationMatrix, 
            Magnum::SceneGraph::Camera3D &camera) override;

        Magnum::Shaders::Phong &_shader;
        Magnum::GL::Mesh &_mesh;
        Magnum::GL::Texture2D &_texture;
    };
} // namespace Reload
#endif