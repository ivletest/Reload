#ifndef _Reload_Program_h_
#define _Reload_Program_h_

#include <memory>
#include <Magnum/Magnum.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Corrade/Containers/Optional.h>


#include "Types.h"
#include "Scene/Scene.h"

namespace Reload
{
    class Game : public Magnum::Platform::Application
    {
    public:
        explicit Game(const Arguments &arguments);
        ~Game() = default;

    private:
        virtual void drawEvent() override;
        virtual void viewportEvent(ViewportEvent &event) override;
        virtual void mousePressEvent(MouseEvent &event) override;
        virtual void mouseReleaseEvent(MouseEvent &event) override;
        virtual void mouseMoveEvent(MouseMoveEvent &event) override;
        virtual void mouseScrollEvent(MouseScrollEvent &event) override;
        virtual void keyPressEvent(KeyEvent &event) override;
    };
} // namespace Reload

#endif