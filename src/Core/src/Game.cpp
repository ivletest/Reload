#ifndef _Reload_Game_h_
#define _Reload_Game_h_

#include "Game.h"

#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/DebugOutput.h>

using namespace Magnum;
using namespace Math::Literals;

namespace Reload
{
    Game::Game(const Arguments &arguments)
#ifdef SDL2_APPLICATION
        : Platform::Sdl2Application{arguments, NoCreate}
#elif EMSCRIPTEN_APPLICATION
        : Platform::EmpscriptenApplication{arguments, NoCreate}
#elif ANDROID_APPLICATION 
        : Platform::AndroidApplication{arguments, NoCreate}
#endif
    {
        Configuration conf;
        GLConfiguration glConf;

        conf.setTitle("Reload");
        conf.setSize({1280, 768});
        conf.setWindowFlags(Configuration::WindowFlag::Resizable);

        glConf.setSampleCount(16);
        glConf.setSrgbCapable(true);

#ifndef NDEBUG
        glConf.addFlags(GLConfiguration::Flag::Debug);
#endif

        if (!tryCreate(conf, glConf))
        {
            create(conf, glConf.setSampleCount(0));
        }

#ifndef NDEBUG
		GL::Renderer::enable(GL::Renderer::Feature::DebugOutput);
		GL::Renderer::enable(GL::Renderer::Feature::DebugOutputSynchronous);
		GL::DebugOutput::setDefaultCallback();
		GL::DebugOutput::setEnabled(GL::DebugOutput::Source::Api, GL::DebugOutput::Type::Other, {131185}, false);
#endif

		GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
        GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
		GL::Renderer::setClearColor(0xa5c9ea_rgbf);
		GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add, GL::Renderer::BlendEquation::Add);
		GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha,
		                               GL::Renderer::BlendFunction::OneMinusSourceAlpha);

        /* Loop at 60 Hz max */
        setSwapInterval(1);
        setMinimalLoopPeriod(16);

        _scene.emplace(windowSize(), framebufferSize());
    }

    void Game::drawEvent()
    {
        GL::defaultFramebuffer.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);
        GL::defaultFramebuffer.clearColor(Color3(0.21, 0.23, 0.2));

        bool camChanged = false;
        _scene->OnDrawEvent(camChanged);

        swapBuffers();

        if(camChanged)
        {
            redraw();
        }
    }

        void Game::viewportEvent(ViewportEvent &event)
    { 
        _scene->OnViewportEvent(event);
    }

    void Game::mousePressEvent(MouseEvent &event)
    { 
        _scene->OnMousePress(event);
    }

    void Game::mouseReleaseEvent(MouseEvent &event)
    { 
        _scene->OnMouseRelease(event);
    }

    void Game::mouseScrollEvent(MouseScrollEvent &event)
    {
        _scene->OnMouseScroll(event);
        redraw();
    }

    void Game::mouseMoveEvent(MouseMoveEvent &event)
    {
        _scene->OnMouseMove(event);
        redraw();
    }

    void Game::keyPressEvent(KeyEvent &event)
    {
        _scene->OnKeyPress(event);
        redraw();
    }

} // namespace Reload

#endif