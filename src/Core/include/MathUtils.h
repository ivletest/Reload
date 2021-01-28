#ifndef _Reload_MathUtils_h
#define _Reload_MathUtils_h

#include <Magnum/Magnum.h>

#include "Types.h"

namespace Reload
{
    struct Ray
    {
        Magnum::Vector3 origin;
        Magnum::Vector3 direction;
    };

    Ray getCameraToViewportRay(
        Magnum::SceneGraph::Camera3D &camera, 
        const Magnum::Vector2 &screenPoint);

} // namespace Reload
#endif