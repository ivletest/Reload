// #ifndef __UI_H
// #define __UI_H

// #include <SDL2/SDL.h>
// #include <SDL2/SDL_opengl.h>

// #include "common.h"

// typedef struct {
//     SDL_Window *window;
//     struct nk_context *ctx;
//     struct nk_font_atlas* atlas;
// } UI;

// extern UI ui;

// extern void ui_Init(SDL_Window * window);
// extern void ui_Cleanup(void);

// extern void nk_sdl_font_stash_begin(struct nk_font_atlas **atlas);
// extern void nk_sdl_font_stash_end(void);
// extern int  nk_sdl_handle_event(SDL_Event *evt);
// extern void nk_sdl_render(enum nk_anti_aliasing , int max_vertex_buffer, int max_element_buffer);
// extern void nk_sdl_shutdown(void);
// extern void nk_sdl_device_destroy(void);
// extern void nk_sdl_device_create(void);

// #endif // !__UI_H