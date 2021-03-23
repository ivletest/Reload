//
// Created by ivan on 19.3.21.
//

#ifndef RELOAD_RENDERSYSTEM_H
#define RELOAD_RENDERSYSTEM_H

#include "RenderBackend.h"

class RenderSystem {
public:
                    RenderSystem();
                    ~RenderSystem();

    void            Init();                                                     // Initializes the rendering system.
    void            Shutdown();                                                 // Shuts down the rendering system.
    void            PrepareScene();
    void            PresentScene();
private:
    RenderBackend   m_backend;
    bool            m_Initialized;
};


#endif //RELOAD_RENDERSYSTEM_H
