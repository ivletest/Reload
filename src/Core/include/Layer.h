#ifndef _Reload_Layer_h_
#define _Reload_Layer_h_

#include <string>

#include "Types.h"

namespace Reload {
	
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}

		virtual void OnViewportEvent(ViewportEvent& event) {}
        virtual void OnMousePress(MouseEvent& event) {}
        virtual void OnMouseRelease(MouseEvent& event) {}
        virtual void OnMouseMove(MouseMoveEvent& event) {}
        virtual void OnMouseScroll(MouseScrollEvent& event) {}
        virtual void OnKeyPress(KeyEvent& event) {}
		virtual void OnTextInput(TextInputEvent &event) {}
		
		virtual void OnImGuiRender() {}
		virtual void OnEvent(InputEvent& event) {}

		inline const std::string& GetName() const { return _debugName; }
	protected:
		std::string _debugName;
	};
}
#endif