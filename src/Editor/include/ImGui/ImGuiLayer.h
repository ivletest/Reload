#ifndef _ReloadEditor_ImGuiLayer_h_
#define _ReloadEditor_ImGuiLayer_h_

#include <string>
#include <imgui.h>
#include <Magnum/ImGuiIntegration/Context.hpp>

#include "Types.h"

#include "Layer.h"

namespace ReloadEditor
{
	class ImGuiLayer : Reload::Layer
	{
	public:
		ImGuiLayer();
		ImGuiLayer(const std::string &name);
		virtual ~ImGuiLayer();

		void Begin();
		void End();

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnViewportEvent(ViewportEvent& event) override;
        virtual void OnMousePress(MouseEvent& event) override;
        virtual void OnMouseRelease(MouseEvent& event) override;
        virtual void OnMouseMove(MouseMoveEvent& event) override;
        virtual void OnMouseScroll(MouseScrollEvent& event) override;
        virtual void OnKeyPress(KeyEvent& event) override;
		virtual void OnTextInput(TextInputEvent &event) override;

	private:
		float m_Time = 0.0f;
		Magnum::ImGuiIntegration::Context _imgui { Magnum::NoCreate };
		Magnum::Vector2i _windowSize;
		Magnum::Vector2i _frameBufferSize;
		Magnum::Vector2 _dpiScaling;
	};
} // namespace ReloadEditor
#endif