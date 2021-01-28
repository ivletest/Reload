#ifndef _Reload_GridRenderer_h_
#define _Reload_GridRenderer_h_

#include <Magnum/Math/Math.h>
#include <Magnum/Resource.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/Shaders/VertexColor.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/DebugTools/ResourceManager.h>

#include "Types.h"

namespace Reload
{
    class GridRenderer : public Magnum::SceneGraph::Drawable3D
    {
    public:
        explicit GridRenderer(
            Object3D &object, 
            Magnum::SceneGraph::DrawableGroup3D *drawables = nullptr);

    private:
        void draw(const Magnum::Matrix4 &transformationMatrix, Magnum::SceneGraph::Camera3D &camera) override;
        Magnum::Color3 getLineColor(
            Magnum::Float i,
            const Magnum::Color3 &baseColor,
            const Magnum::Color3 &axesColor,
            const Magnum::Color3 &subColor);
        Magnum::Resource<Magnum::GL::AbstractShaderProgram, Magnum::Shaders::VertexColor3D> _shader;
        Magnum::Resource<Magnum::GL::Mesh> _mesh;
        Magnum::Resource<Magnum::GL::Buffer> _vertexBuffer;
    };
} // namespace Reload

#endif