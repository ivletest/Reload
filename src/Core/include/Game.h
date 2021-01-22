#ifndef _Reload_Program_h_
#define _Reload_Program_h_

#include <memory>
#include <Magnum/Magnum.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Corrade/Containers/Optional.h>

#ifdef SDL2_APPLICATION
    #include <Magnum/Platform/Sdl2Application.h>
#elif EMSCRIPTEN_APPLICATION
    #include <Magnum/Platform/EmscriptenApplication.h>
#elif ANDROID_APPLICATION 
    #include <Magnum/Platform/AndroidApplication.h>
#endif

#include "Types.h"
#include "Scene/Scene.h"

namespace Reload
{
#ifdef SDL2_APPLICATION
    class Game : public Magnum::Platform::Sdl2Application
#elif EMSCRIPTEN_APPLICATION
    class Game : public Magnum::Platform::EmscriptenApplication
#elif ANDROID_APPLICATION 
    class Game : public Magnum::Platform::AndroidApplication
#endif
    {
    public:
        explicit Game(const Arguments &arguments);
        ~Game() = default;

    private:
        void drawEvent() override;
        void viewportEvent(ViewportEvent &event) override;
        void mousePressEvent(MouseEvent &event) override;
        void mouseReleaseEvent(MouseEvent &event) override;
        void mouseMoveEvent(MouseMoveEvent &event) override;
        void mouseScrollEvent(MouseScrollEvent &event) override;
        void keyPressEvent(KeyEvent &event) override;

        Magnum::Containers::Optional<Scene> _scene {};
    };
} // namespace Reload

#endif