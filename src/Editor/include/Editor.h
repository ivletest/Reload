#ifndef _ReloadEditor_Editor_h_
#define _ReloadEditor_Editor_h_

#include <Magnum/Magnum.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Corrade/Containers/Optional.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/DebugOutput.h>

#include "Types.h"

#include "EditorLayer.h"
#include "ImGui/ImGuiLayer.h"

namespace ReloadEditor
{
    class Editor : public Magnum::Platform::Application
    {
    public:
        explicit Editor(const Arguments &arguments);
        ~Editor() = default;

        static inline Editor &Get() { return *s_instance; }

    private:
        void drawEvent() override;
        void viewportEvent(ViewportEvent &event) override;
        void mousePressEvent(MouseEvent &event) override;
        void mouseReleaseEvent(MouseEvent &event) override;
        void mouseMoveEvent(MouseMoveEvent &event) override;
        void mouseScrollEvent(MouseScrollEvent &event) override;
        void keyPressEvent(KeyEvent &event) override;
        void textInputEvent(TextInputEvent &event) override;

        Magnum::Containers::Optional<EditorLayer> _editor;
        Magnum::Containers::Optional<ImGuiLayer>_imguiLayer;

        static Editor *s_instance;
    };
} // namespace ReloadEditor

#endif