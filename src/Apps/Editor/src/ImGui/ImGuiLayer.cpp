#include "ImGui/ImGuiLayer.h"

#ifdef SDL2_APPLICATION
#include <imgui_impl_sdl.h>
#ifdef OPENGL
#define IMGUI_IMPL_OPENGL_LOADER_GLBINDING2
#include <imgui_impl_opengl3.h>
#endif
#endif
#include <Corrade/Utility/Directory.h>

#include "Editor.h"

using namespace Magnum;

namespace ReloadEditor
{
    ImGuiLayer::ImGuiLayer()
    {
    }

    ImGuiLayer::ImGuiLayer(const std::string &name)
    {
    }

    ImGuiLayer::~ImGuiLayer()
    {
    }

    void ImGuiLayer::OnAttach()
    {
        // Setup Dear ImGui context
        Editor &editor = Editor::Get();

        IMGUI_CHECKVERSION();
        _imgui = ImGuiIntegration::Context(
            Vector2{editor.windowSize()} / editor.dpiScaling(),
            editor.windowSize(),
            editor.framebufferSize());

        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

        auto fontsDir = Corrade::Utility::Directory::join(
            {Corrade::Utility::Directory::current(), "Assets", "Fonts"});

        ImFont *pFont = io.Fonts->AddFontFromFileTTF(
            Corrade::Utility::Directory::join(fontsDir, "IBMPlexMono-Regular.ttf").c_str(),
            18.0f);

        io.FontDefault = io.Fonts->Fonts.back();

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle &style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, style.Colors[ImGuiCol_WindowBg].w);

#ifdef SDL2_APPLICATION
        ImGui_ImplSDL2_InitForOpenGL(Editor::Get().window(), Editor::Get().glContext());
        ImGui_ImplOpenGL3_Init("#version 410");
#endif
    }

    void ImGuiLayer::OnDetach()
    {
#ifdef SDL2_APPLICATION
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
#endif
        ImGui::DestroyContext();
    }

    void ImGuiLayer::OnViewportEvent(ViewportEvent &event)
    {
        _imgui.relayout(
            Vector2{event.windowSize()} / event.dpiScaling(),
            event.windowSize(), event.framebufferSize());
    }

    void ImGuiLayer::OnMousePress(MouseEvent &event)
    {
        if (_imgui.handleMousePressEvent(event))
        {
            return;
        }
    }

    void ImGuiLayer::OnMouseRelease(MouseEvent &event)
    {
        if (_imgui.handleMouseReleaseEvent(event))
        {
            return;
        }
    }

    void ImGuiLayer::OnMouseMove(MouseMoveEvent &event)
    {
        if (_imgui.handleMouseMoveEvent(event))
        {
            return;
        }
    }

    void ImGuiLayer::OnMouseScroll(MouseScrollEvent &event)
    {
        if (_imgui.handleMouseScrollEvent(event))
        {
            event.setAccepted();
            return;
        }
    };

    void ImGuiLayer::OnKeyPress(KeyEvent &event)
    {
        if (_imgui.handleKeyPressEvent(event))
        {
            return;
        }
    };

    void ImGuiLayer::OnTextInput(TextInputEvent &event)
    {
        if (_imgui.handleTextInputEvent(event))
        {
            return;
        }
    }

    void ImGuiLayer::Begin()
    {
#ifdef SDL2_APPLICATION
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(Editor::Get().window());
#endif
        _imgui.newFrame();

        // ImGuizmo::BeginFrame();
        /* Enable text input, if needed */
        if (ImGui::GetIO().WantTextInput && !Editor::Get().isTextInputActive())
        {
            Editor::Get().startTextInput();
        }
        else if (!ImGui::GetIO().WantTextInput && Editor::Get().isTextInputActive())
        {
            Editor::Get().stopTextInput();
        }
    }

    void ImGuiLayer::End()
    {
        /* Update application cursor */
        _imgui.updateApplicationCursor(Editor::Get());

        /* Set appropriate states. If you only draw ImGui, it is sufficient to
       just enable blending and scissor test in the constructor. */
        GL::Renderer::enable(GL::Renderer::Feature::Blending);
        GL::Renderer::enable(GL::Renderer::Feature::ScissorTest);
        GL::Renderer::disable(GL::Renderer::Feature::FaceCulling);
        GL::Renderer::disable(GL::Renderer::Feature::DepthTest);

        _imgui.drawFrame();

        /* Reset state. Only needed if you want to draw something else with
       different state after. */
        GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
        GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
        GL::Renderer::disable(GL::Renderer::Feature::ScissorTest);
        GL::Renderer::disable(GL::Renderer::Feature::Blending);
    }

} // namespace ReloadEditor