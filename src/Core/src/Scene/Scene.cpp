#include "Scene/Scene.h"

#include <Corrade/Utility/Directory.h>
#include <Corrade/Utility/String.h>
#include <Magnum/GL/DefaultFramebuffer.h>

using namespace Magnum;
using namespace Math::Literals;

namespace Reload
{
    const std::string GLTF_IMPORTER = "TinyGltfImporter";

    Scene::Scene(
        const Vector2i windowSize,
        const Vector2i frameBufferSize)
    {
        /* Set up the camera */
        {
            const Vector3 eye = Vector3::zAxis(-10.0f);
            const Vector3 center{};
            const Vector3 up = Vector3::yAxis();
            Deg fov = 45.0_degf;

            _cameraRig.emplace(_scene, eye, center, up, fov, windowSize, frameBufferSize);
        }

        /* Setup renderer and shader defaults */
        _coloredShader
            .setAmbientColor(0x000000_rgbf)
            .setLightColor(0xf6e58d_rgbf)
            .setSpecularColor(0xffffff_rgbf)
            .setShininess(10.0f);
        _texturedShader
            .setAmbientColor(0x000000_rgbf)
            .setLightColor(0xf6e58d_rgbf)
            .setSpecularColor(0x111111_rgbf)
            .setShininess(10.0f);

        PluginManager::Manager<Trade::AbstractImporter> manager;

        if (!(manager.load(GLTF_IMPORTER) & PluginManager::LoadState::Loaded))
        {
            Utility::Error{}
                << "The requested plugin" << GLTF_IMPORTER << "cannot be loaded.";
        }

        Containers::Pointer<Trade::AbstractImporter> gltfImporter =
            manager.instantiate(GLTF_IMPORTER);

        CORRADE_INTERNAL_ASSERT(gltfImporter);

        auto modelsDir = Corrade::Utility::Directory::join(
            {Corrade::Utility::Directory::current(), "Assets", "models", "scenes", "rooftop"});

        /* Load file */
        if (!gltfImporter->openFile(Corrade::Utility::Directory::join(
                modelsDir,
                "rooftop-degenerate.glb")))
        {
            std::exit(4);
        }

        /* Load all textures. Textures that fail to load will be NullOpt. */
        _textures = Containers::Array<Containers::Optional<GL::Texture2D>>{gltfImporter->textureCount()};
        for (UnsignedInt i = 0; i != gltfImporter->textureCount(); ++i)
        {
            Debug{} << "Importing texture" << i << gltfImporter->textureName(i);
            Containers::Optional<Trade::TextureData> textureData = gltfImporter->texture(i);
            if (!textureData || textureData->type() != Trade::TextureData::Type::Texture2D)
            {
                Warning{} << "Cannot load texture properties, skipping";
                continue;
            }

            Debug{} << "Importing image" << textureData->image() << gltfImporter->image2DName(textureData->image());

            Containers::Optional<Trade::ImageData2D> imageData = gltfImporter->image2D(textureData->image());
            GL::TextureFormat format;
            if (imageData && imageData->format() == PixelFormat::RGB8Unorm)
            {
                format = GL::TextureFormat::RGB8;
            }
            else if (imageData && imageData->format() == PixelFormat::RGBA8Unorm)
            {
                format = GL::TextureFormat::RGBA8;
            }
            else
            {
                Warning{} << "Cannot load texture image, skipping";
                continue;
            }

            /* Configure the texture */
            GL::Texture2D texture;
            texture
                .setMagnificationFilter(textureData->magnificationFilter())
                .setMinificationFilter(textureData->minificationFilter(), textureData->mipmapFilter())
                .setWrapping(textureData->wrapping().xy())
                .setStorage(Math::log2(imageData->size().max()) + 1, format, imageData->size())
                .setSubImage(0, {}, *imageData)
                .generateMipmap();

            _textures[i] = std::move(texture);

            /* Load all materials. Materials that fail to load will be NullOpt. The
           data will be stored directly in objects later, so save them only
           temporarily. */
            Containers::Array<Containers::Optional<Trade::PhongMaterialData>> materials{gltfImporter->materialCount()};
            for (UnsignedInt i = 0; i != gltfImporter->materialCount(); ++i)
            {
                Debug{} << "Importing material" << i << gltfImporter->materialName(i);

                Containers::Optional<Trade::MaterialData> materialData = gltfImporter->material(i);
                if (!materialData || !(materialData->types() & Trade::MaterialType::Phong))
                {
                    Warning{} << "Cannot load material, skipping";
                    continue;
                }

                materials[i] = std::move(static_cast<Trade::PhongMaterialData &>(*materialData));
            }

            /* Load all meshes. Meshes that fail to load will be NullOpt. */
            _meshes = Containers::Array<Containers::Optional<GL::Mesh>>{gltfImporter->meshCount()};
            for (UnsignedInt i = 0; i != gltfImporter->meshCount(); ++i)
            {
                Debug{} << "Importing mesh" << i << gltfImporter->meshName(i);

                Containers::Optional<Trade::MeshData> meshData = gltfImporter->mesh(i);
                if (!meshData || !meshData->hasAttribute(Trade::MeshAttribute::Normal) || meshData->primitive() != MeshPrimitive::Triangles)
                {
                    Warning{} << "Cannot load the mesh, skipping";
                    continue;
                }

                /* Compile the mesh */
                _meshes[i] = MeshTools::compile(*meshData);
            }

            /* Load the scene */
            if (gltfImporter->defaultScene() != -1)
            {
                Debug{} << "Adding default scene" << gltfImporter->sceneName(gltfImporter->defaultScene());

                Containers::Optional<Trade::SceneData> sceneData = gltfImporter->scene(gltfImporter->defaultScene());
                if (!sceneData)
                {
                    Error{} << "Cannot load scene, exiting";
                    return;
                }

                /* Recursively add all children */
                for (UnsignedInt objectId : sceneData->children3D())
                {
                    addObject(*gltfImporter, materials, _scene, objectId);
                }

                /* The format has no scene support, display just the first loaded mesh with
       a default material and be done with it */
            }
            else if (!_meshes.empty() && _meshes[0])
            {
                new ColoredDrawable{_scene, _coloredShader, *_meshes[0], 0xffffff_rgbf, _drawables};
            }
        }
    }

    void Scene::addObject(
        Trade::AbstractImporter &importer,
        Containers::ArrayView<const Containers::Optional<Trade::PhongMaterialData>> materials,
        Object3D &parent,
        UnsignedInt i)
    {
        Debug{} << "Importing object" << i << importer.object3DName(i);
        Containers::Pointer<Trade::ObjectData3D> objectData = importer.object3D(i);
        if (!objectData)
        {
            Error{} << "Cannot import object, skipping";
            return;
        }

        /* Add the object to the scene and set its transformation */
        auto *object = new Object3D{&parent};
        object->setTransformation(objectData->transformation());

        /* Add a drawable if the object has a mesh and the mesh is loaded */
        if (objectData->instanceType() == Trade::ObjectInstanceType3D::Mesh && objectData->instance() != -1 && _meshes[objectData->instance()])
        {
            const Int materialId = static_cast<Trade::MeshObjectData3D *>(objectData.get())->material();

            /* Material not available / not loaded, use a default material */
            if (materialId == -1 || !materials[materialId])
            {
                new ColoredDrawable{*object, _coloredShader, *_meshes[objectData->instance()], 0xffffff_rgbf, _drawables};

                /* Textured material. If the texture failed to load, again just use a
           default colored material. */
            }
            else if (materials[materialId]->hasAttribute(Trade::MaterialAttribute::DiffuseTexture))
            {
                Containers::Optional<GL::Texture2D> &texture = _textures[materials[materialId]->diffuseTexture()];
                if (texture)
                {
                    new TexturedDrawable{*object, _texturedShader, *_meshes[objectData->instance()], *texture, _drawables};
                }
                else
                {
                    new ColoredDrawable{*object, _coloredShader, *_meshes[objectData->instance()], 0xffffff_rgbf, _drawables};
                }

                /* Color-only material */
            }
            else
            {
                new ColoredDrawable{*object, _coloredShader, *_meshes[objectData->instance()], materials[materialId]->diffuseColor(), _drawables};
            }
        }

        /* Recursively add children */
        for (std::size_t id : objectData->children())
        {
            addObject(importer, materials, *object, id);
        }
    }

    void Scene::OnDrawEvent(bool &camChanged)
    {
        /* Call update in every frame. This will do nothing if the camera
           has not been changed. Otherwise, camera transformation will be
           propagated into the camera objects. */
        camChanged = _cameraRig->update();
        _cameraRig->draw(_drawables);
    }

    void Scene::OnViewportEvent(ViewportEvent &event)
    {
        GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
        _cameraRig->reshape(event.windowSize(), event.framebufferSize());
    }

    void Scene::OnMousePress(MouseEvent &event)
    {
        /* Enable mouse capture so the mouse can drag outside of the window */
        /** @todo replace once https://github.com/mosra/magnum/pull/419 is in */
        SDL_CaptureMouse(SDL_TRUE);

        _cameraRig->initTransformation(event.position());

        event.setAccepted();
    }

    void Scene::OnMouseRelease(MouseEvent &event)
    {
        /* Disable mouse capture again */
        /** @todo replace once https://github.com/mosra/magnum/pull/419 is in */
        SDL_CaptureMouse(SDL_FALSE);
    }

    void Scene::OnMouseScroll(MouseScrollEvent &event)
    {
        const Float delta = event.offset().y();
        if (Math::abs(delta) < 1.0e-2f)
            return;

        _cameraRig->zoom(delta);

        event.setAccepted();
    }

    void Scene::OnMouseMove(MouseMoveEvent &event)
    {
        if (!event.buttons())
        {
            return;
        }

        if (event.modifiers() & MouseMoveEvent::Modifier::Shift)
        {
            _cameraRig->translate(event.position());
        }
        else
        {
            _cameraRig->rotate(event.position());
        }

        event.setAccepted();
    }

    void Scene::OnKeyPress(KeyEvent &event)
    {
        switch (event.key())
        {
        case KeyEvent::Key::L:
            if (_cameraRig->inertia() > 0.0f)
            {
                Debug{} << "Lagging disabled";
                _cameraRig->setInertia(0.0f);
            }
            else
            {
                Debug{} << "Lagging enabled";
                _cameraRig->setInertia(0.85f);
            }
            break;
        case KeyEvent::Key::R:
            _cameraRig->reset();
            break;

        default:
            return;
        }

        event.setAccepted();
    }

    //------------------------------------------------------------------------------
    // Drawables
    //------------------------------------------------------------------------------

    void ColoredDrawable::draw(const Matrix4 &transformationMatrix, SceneGraph::Camera3D &camera)
    {
        _shader
            .setDiffuseColor(_color)
            .setLightPositions({{-3.0f, 70.0f, 50.0f, 0.0f}})
            .setTransformationMatrix(transformationMatrix)
            .setNormalMatrix(transformationMatrix.normalMatrix())
            .setProjectionMatrix(camera.projectionMatrix())
            .draw(_mesh);
    }

    void TexturedDrawable::draw(const Matrix4 &transformationMatrix, SceneGraph::Camera3D &camera)
    {
        _shader
            .setLightPositions({{-3.0f, 70.0f, 50.0f, 0.0f}})
            .setTransformationMatrix(transformationMatrix)
            .setNormalMatrix(transformationMatrix.normalMatrix())
            .setProjectionMatrix(camera.projectionMatrix())
            .bindDiffuseTexture(_texture)
            .draw(_mesh);
    }
} // namespace Reload