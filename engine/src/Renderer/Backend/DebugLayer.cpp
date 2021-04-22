#include "DebugLayer.h"

// #include "../ui.h"

void debugLayer_Init(void)
{
    // memset(&debugInfo, 0, sizeof(DebugLayerInfo));
    // ui_Init(window);
}

void debugLayer_Draw(void)
{
    // struct nk_context *ctx = ui.ctx;
    // int winMinW, winMinH, winMaxW, winMaxH;

    // SDL_GetWindowMinimumSize(window, &winMinW, &winMinH);
    // SDL_GetWindowMaximumSize(window, &winMaxW, &winMaxH);

    // if (nk_begin(ctx, "Main", nk_rect(winMinW, winMinH, winMaxW, winMaxH + 40),
    //              NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND))
    // {

    //     nk_menubar_begin(ctx);
    //     nk_layout_row_static(ctx, 40, 40, 1);
    //     if (nk_menu_begin_label(ctx, "Main", NK_TEXT_LEFT, nk_vec2(120, 200)))
    //     {
    //         nk_layout_row_dynamic(ctx, 25, 1);
    //         if (nk_menu_item_label(ctx, "Exit", NK_TEXT_LEFT))
    //         {
    //             exit(1);
    //         }
    //     }
    //     nk_menubar_end(ctx);
    // }
    // nk_end(ctx);
}

void debugLayer_Cleanup(void)
{
    // ui_Cleanup();
}