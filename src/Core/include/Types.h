#ifndef _Types_h_
#define _Types_h_

#include <Magnum/SceneGraph/MatrixTransformation3D.h>

#ifdef SDL2_APPLICATION
    #include <Magnum/Platform/Sdl2Application.h>
#elif EMSCRIPTEN_APPLICATION
    #include <Magnum/Platform/EmscriptenApplication.h>
#elif ANDROID_APPLICATION 
    #include <Magnum/Platform/AndroidApplication.h>
#endif

typedef Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation3D> Object3D;
typedef Magnum::SceneGraph::Scene<Magnum::SceneGraph::MatrixTransformation3D> Scene3D;

#ifdef SDL2_APPLICATION
    typedef Magnum::Platform::Sdl2Application::InputEvent InputEvent;
    typedef Magnum::Platform::Sdl2Application::KeyEvent KeyEvent;
    typedef Magnum::Platform::Sdl2Application::MouseEvent MouseEvent;
    typedef Magnum::Platform::Sdl2Application::MouseMoveEvent MouseMoveEvent;
    typedef Magnum::Platform::Sdl2Application::MouseScrollEvent MouseScrollEvent;
    typedef Magnum::Platform::Sdl2Application::MultiGestureEvent MultiGestureEvent;
    typedef Magnum::Platform::Sdl2Application::TextEditingEvent TextEditingEvent;
    typedef Magnum::Platform::Sdl2Application::TextInputEvent TextInputEvent;
    typedef Magnum::Platform::Sdl2Application::ViewportEvent ViewportEvent;
#elif EMSCRIPTEN_APPLICATION
    typedef Magnum::Platform::EmscriptenApplication::InputEvent InputEvent;
    typedef Magnum::Platform::EmscriptenApplication::KeyEvent KeyEvent;
    typedef Magnum::Platform::EmscriptenApplication::MouseEvent MouseEvent;
    typedef Magnum::Platform::EmscriptenApplication::MouseMoveEvent MouseMoveEvent;
    typedef Magnum::Platform::EmscriptenApplication::MouseScrollEvent MouseScrollEvent;
    typedef Magnum::Platform::EmscriptenApplication::MultiGestureEvent MultiGestureEvent;
    typedef Magnum::Platform::EmscriptenApplication::TextEditingEvent TextEditingEvent;
    typedef Magnum::Platform::EmscriptenApplication::TextInputEvent TextInputEvent;
    typedef Magnum::Platform::EmscriptenApplication::ViewportEvent ViewportEvent;
#elif ANDROID_APPLICATION 
    typedef Magnum::Platform::AndroidApplication::InputEvent InputEvent;
    typedef Magnum::Platform::AndroidApplication::KeyEvent KeyEvent;
    typedef Magnum::Platform::AndroidApplication::MouseEvent MouseEvent;
    typedef Magnum::Platform::AndroidApplication::MouseMoveEvent MouseMoveEvent;
    typedef Magnum::Platform::AndroidApplication::MouseScrollEvent MouseScrollEvent;
    typedef Magnum::Platform::AndroidApplication::MultiGestureEvent MultiGestureEvent;
    typedef Magnum::Platform::AndroidApplication::TextEditingEvent TextEditingEvent;
    typedef Magnum::Platform::AndroidApplication::TextInputEvent TextInputEvent;
    typedef Magnum::Platform::AndroidApplication::ViewportEvent ViewportEvent;
#endif

#endif