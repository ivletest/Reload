#include "Editor.h"
#include <imgui.h>

using namespace Magnum;
using namespace Math::Literals;

namespace ReloadEditor
{
    Editor *Editor::s_instance = nullptr;

    Editor::Editor(const Arguments &arguments) : Platform::Application{arguments, NoCreate}
    {
        s_instance = this;

        Configuration conf;
        GLConfiguration glConf;

        conf.setTitle("Reload Editor");
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

        // _scene.emplace(windowSize(), framebufferSize());
        _imguiLayer.emplace("EditorUI");
        _imguiLayer->OnAttach();
        _editor.emplace();
        _editor->OnAttach();

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
    }

    void Editor::drawEvent()
    {
        GL::defaultFramebuffer.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);
        GL::defaultFramebuffer.clearColor(Color3(0.21, 0.23, 0.2));

        bool camChanged = false;
        // _scene->DrawEvent(camChanged);
        _editor->OnUpdate();
        _editor->GetScene()->OnDrawEvent(camChanged);

        _imguiLayer->Begin();
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
        ImGui::ShowDemoWindow();
        // _editor->OnImGuiRender();
        _imguiLayer->End();
        
        swapBuffers();
        redraw();
        
        // if (camChanged)
        // {
        //     redraw();    
        // }
    }

    void Editor::viewportEvent(ViewportEvent &event)
    {
        GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
        
        _imguiLayer->OnViewportEvent(event);
        _editor->OnViewportEvent(event);
    }

    void Editor::mousePressEvent(MouseEvent &event)
    {
        _imguiLayer->OnMousePress(event);
        _editor->OnMousePress(event);
    }

    void Editor::mouseReleaseEvent(MouseEvent &event)
    {
        _imguiLayer->OnMouseRelease(event);
        _editor->GetScene()->OnMouseRelease(event);
    }

    void Editor::mouseScrollEvent(MouseScrollEvent &event)
    {
        _imguiLayer->OnMouseScroll(event);
        _editor->OnMouseScroll(event);
        redraw();
    }

    void Editor::mouseMoveEvent(MouseMoveEvent &event)
    {
        _imguiLayer->OnMouseMove(event);
        _editor->OnMouseMove(event);
        redraw();
    }

    void Editor::keyPressEvent(KeyEvent &event)
    {
        _imguiLayer->OnKeyPress(event);
        _editor->OnKeyPress(event);
        redraw();
    }

    void Editor::textInputEvent(TextInputEvent &event)
    {
        _imguiLayer->OnTextInput(event);
    }

} // namespace ReloadEditor