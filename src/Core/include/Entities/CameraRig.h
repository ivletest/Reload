#ifndef _Reload_CameraRig_h_
#define _Reload_CameraRig_h_

#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/Magnum.h>
#include <Magnum/Math/Functions.h>
#include <Magnum/Math/DualQuaternion.h>
#include <Magnum/Math/Vector2.h>
#include <Magnum/Math/Vector3.h>
#include <Magnum/SceneGraph/AbstractTranslationRotation3D.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/Scene.h>

#include "Types.h"

namespace Reload
{
     Magnum::Quaternion ndcToArcBall(const Magnum::Vector2 &p);

     class CameraRig
     {
     public:
          /* Implementation of the constructor is here on purpose because of the
      template */
          template <class Transformation>
          CameraRig(
              Magnum::SceneGraph::Scene<Transformation> &scene,
              const Magnum::Vector3 &eye,
              const Magnum::Vector3 &lookAt,
              const Magnum::Vector3 &upDir,
              Magnum::Deg fov,
              const Magnum::Vector2i &windowSize,
              const Magnum::Vector2i &viewportSize)
              : _fov{fov}, _windowSize{windowSize}
          {
               setViewParameters(eye, lookAt, upDir);
               auto *cameraObject = new Magnum::SceneGraph::Object<Transformation>{&scene};

               (*(_camera = new Magnum::SceneGraph::Camera3D{*cameraObject}))
                   .setAspectRatioPolicy(Magnum::SceneGraph::AspectRatioPolicy::Extend)
                   .setProjectionMatrix(Magnum::Matrix4::perspectiveProjection(
                       fov, Magnum::Vector2{windowSize}.aspectRatio(), 0.01f, 1000.0f))
                   .setViewport(viewportSize);

               /* Save the abstract transformation interface and initialize the
           camera position through that */
               (*(_cameraObject = cameraObject))
                   .rotate(transformation().rotation())
                   .translate(transformation().translation());
          }

          ~CameraRig() = default;
          /* Set the camera view parameters: eye position, view center, up
           direction */
          void setViewParameters(
              const Magnum::Vector3 &eye,
              const Magnum::Vector3 &viewCenter,
              const Magnum::Vector3 &upDir);

          /* Reset the camera to its initial position, view center, and up dir */
          void reset();

          /* Update screen size after the window has been resized */
          void reshape(
              const Magnum::Vector2i &windowSize,
              const Magnum::Vector2i &viewportSize);

          /* Update any unfinished transformation due to inertia, return true if
           the camera matrices have changed */
          bool updateTransformation();

          /* Get/set the amount of inertia such that the camera will (slowly)
           smoothly navigate. Inertia must be in [0, 1) */
          Magnum::Float inertia() const { return _inertia; }

          void setInertia(Magnum::Float inertia);

          /* Initialize the first (screen) position for camera
           transformation. This should be called in pressed event. */
          void initTransformation(const Magnum::Vector2i &pos);

          /* Rotate the camera from the previous (screen) position to the
           current (screen) position */
          void rotate(const Magnum::Vector2i &pos);

          /* Translate the camera from the previous (screen) position to
           the current (screen) position */
          void translate(const Magnum::Vector2i &pos);

          /* Translate the camera by the delta amount of (NDC) position.
           Note that NDC position must be in [-1, -1] to [1, 1]. */
          void translateDelta(const Magnum::Vector2 &translationNDC);

          /* Zoom the camera (positive delta = zoom in, negative = zoom out) */
          void zoom(Magnum::Float delta);

          /* Field of view */
          Magnum::Deg fov() const { return _fov; }

          /* Get the camera's view transformation as a qual quaternion */
          const Magnum::DualQuaternion &view() const { return _view; }

          /* Get the camera's view transformation as a matrix */
          Magnum::Matrix4 viewMatrix() const { return _view.toMatrix(); }

          /* Get the camera's inverse view matrix (which also produces
           transformation of the camera) */
          Magnum::Matrix4 inverseViewMatrix() const { return _inverseView.toMatrix(); }

          /* Get the camera's transformation as a dual quaternion */
          const Magnum::DualQuaternion &transformation() const { return _inverseView; }

          /* Get the camera's transformation matrix */
          Magnum::Matrix4 transformationMatrix() const { return _inverseView.toMatrix(); }

          /* Return the distance from the camera position to the center view */
          Magnum::Float viewDistance() const { return Magnum::Math::abs(_targetZooming); }

          /* Update the SceneGraph camera if arcball has been changed */
          bool update();

          /* Draw objects using the internal scenegraph camera */
          void draw(Magnum::SceneGraph::DrawableGroup3D &drawables);

          /* Accessor to the raw camera object */
          Magnum::SceneGraph::Camera3D &camera() const { return *_camera; }

     private:
          Magnum::SceneGraph::AbstractTranslationRotation3D *_cameraObject{};
          Magnum::SceneGraph::Camera3D *_camera{};

          /* Update the camera transformations */
          void updateInternalTransformations();

          /* Transform from screen coordinate to NDC - normalized device
           coordinate. The top-left of the screen corresponds to [-1, 1] NDC,
           and the bottom right is [1, -1] NDC. */
          Magnum::Vector2 screenCoordToNDC(const Magnum::Vector2i &pos) const;

          Magnum::Deg _fov;
          Magnum::Vector2i _windowSize;

          Magnum::Vector2 _prevPosNDC;
          Magnum::Float _inertia{};

          Magnum::Vector3 _targetPosition, _currentPosition, _positionT0;
          Magnum::Quaternion _targetQRotation, _currentQRotation, _qRotationT0;
          Magnum::Float _targetZooming, _currentZooming, _zoomingT0;
          Magnum::DualQuaternion _view, _inverseView;
     };
} // namespace Reload

#endif