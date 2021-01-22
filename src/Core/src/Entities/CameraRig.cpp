#include "Entities/CameraRig.h"

#include <Magnum/Math/Matrix3.h>

using namespace Magnum;
using namespace Math::Literals;

namespace Reload
{
    /* Project a point in NDC onto the arcball sphere */
    Quaternion ndcToArcBall(const Vector2 &p)
    {
        const Float dist = Math::dot(p, p);

        /* Point is on sphere */
        if (dist <= 1.0f)
            return {{p.x(), p.y(), Math::sqrt(1.0f - dist)}, 0.0f};

        /* Point is outside sphere */
        else
        {
            const Vector2 proj = p.normalized();
            return {{proj.x(), proj.y(), 0.0f}, 0.0f};
        }
    }

    void CameraRig::setViewParameters(
        const Vector3 &eye,
        const Vector3 &lookAt,
        const Vector3 &upDir)
    {
        const Vector3 dir = lookAt - eye;
        Vector3 zAxis = dir.normalized();
        Vector3 xAxis = (Math::cross(zAxis, upDir.normalized())).normalized();
        Vector3 yAxis = (Math::cross(xAxis, zAxis)).normalized();
        xAxis = (Math::cross(zAxis, yAxis)).normalized();

        _targetPosition = -lookAt;
        _targetZooming = -dir.length();
        _targetQRotation = Quaternion::fromMatrix(
                               Matrix3x3{xAxis, yAxis, -zAxis}.transposed())
                               .normalized();

        _positionT0 = _currentPosition = _targetPosition;
        _zoomingT0 = _currentZooming = _targetZooming;
        _qRotationT0 = _currentQRotation = _targetQRotation;

        updateInternalTransformations();
    }

    void CameraRig::reset()
    {
        _targetPosition = _positionT0;
        _targetZooming = _zoomingT0;
        _targetQRotation = _qRotationT0;
    }

    void CameraRig::setInertia(const Float inertia)
    {
        CORRADE_INTERNAL_ASSERT(inertia >= 0.0f && inertia < 1.0f);
        _inertia = inertia;
    }

    void CameraRig::initTransformation(const Vector2i &pos)
    {
        _prevPosNDC = screenCoordToNDC(pos);
    }

    void CameraRig::rotate(const Vector2i &pos)
    {
        const Vector2 posNDC = screenCoordToNDC(pos);
        const Quaternion currentQRotation = ndcToArcBall(posNDC);
        const Quaternion prevQRotation = ndcToArcBall(_prevPosNDC);
        _prevPosNDC = posNDC;
        _targetQRotation =
            (currentQRotation * prevQRotation * _targetQRotation).normalized();
    }

    void CameraRig::translate(const Vector2i &pos)
    {
        const Vector2 posNDC = screenCoordToNDC(pos);
        const Vector2 translationNDC = posNDC - _prevPosNDC;
        _prevPosNDC = posNDC;
        translateDelta(translationNDC);
    }

    void CameraRig::translateDelta(const Vector2 &translationNDC)
    {
        /* Half size of the screen viewport at the view center and perpendicular
       with the viewDir */
        const Float hh = Math::abs(_targetZooming) * Math::tan(_fov * 0.5f);
        const Float hw = hh * Vector2{_windowSize}.aspectRatio();
        
        Debug{} << "Fov: " << _fov << "Target zoom" << _targetZooming;

        _targetPosition += _inverseView.transformVector(
            {translationNDC.x() * hw, translationNDC.y() * hh, 0.0f});
    }

    void CameraRig::zoom(const Float delta)
    {
        _targetZooming += delta;
    }

    bool CameraRig::updateTransformation()
    {
        const Vector3 diffViewCenter = _targetPosition - _currentPosition;
        const Quaternion diffRotation = _targetQRotation - _currentQRotation;
        const Float diffZooming = _targetZooming - _currentZooming;

        const Float dViewCenter = Math::dot(diffViewCenter, diffViewCenter);
        const Float dRotation = Math::dot(diffRotation, diffRotation);
        const Float dZooming = diffZooming * diffZooming;

        /* Nothing change */
        if (dViewCenter < 1.0e-10f &&
            dRotation < 1.0e-10f &&
            dZooming < 1.0e-10f)
        {
            return false;
        }

        /* Nearly done: just jump directly to the target */
        if (dViewCenter < 1.0e-6f &&
            dRotation < 1.0e-6f &&
            dZooming < 1.0e-6f)
        {
            _currentPosition = _targetPosition;
            _currentQRotation = _targetQRotation;
            _currentZooming = _targetZooming;

            /* Interpolate between the current transformation and the target
               transformation */
        }
        else
        {
            const Float t = 1 - _inertia;
            _currentPosition = Math::lerp(_currentPosition, _targetPosition, t);
            _currentZooming = Math::lerp(_currentZooming, _targetZooming, t);
            _currentQRotation = Math::slerpShortestPath(
                _currentQRotation, _targetQRotation, t);
        }

        updateInternalTransformations();
        return true;
    }

    void CameraRig::updateInternalTransformations()
    {
        _view = DualQuaternion::translation(Vector3::zAxis(_currentZooming)) *
                DualQuaternion{_currentQRotation} *
                DualQuaternion::translation(_currentPosition);
        _inverseView = _view.inverted();
    }

    Vector2 CameraRig::screenCoordToNDC(const Vector2i &pos) const
    {
        return {pos.x() * 2.0f / _windowSize.x() - 1.0f,
                1.0f - 2.0f * pos.y() / _windowSize.y()};
    }

    /* Update screen and viewport size after the window has been resized */
    void CameraRig::reshape(const Vector2i &windowSize, const Vector2i &viewportSize)
    {
        _windowSize = windowSize;
        _camera->setViewport(viewportSize);
    }

    bool CameraRig::update()
    {
        /* call the internal update */
        if (!updateTransformation())
        {
            return false;
        }

        (*_cameraObject)
            .resetTransformation()
            .rotate(transformation().rotation())
            .translate(transformation().translation());
        return true;
    }

    void CameraRig::draw(SceneGraph::DrawableGroup3D &drawables)
    {
        _camera->draw(drawables);
    }

} // namespace Reload