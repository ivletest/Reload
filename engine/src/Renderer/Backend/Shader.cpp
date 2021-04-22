//#include "Shader.h"
//
//#include <SDL2/SDL_filesystem.h>
//#include <SDL2/SDL_rwops.h>
//
////==============================================================================
//// Shader
////==============================================================================
//
//#define MAX_SHADER_TYPES 4
//
//static bool shader_isInit = false;
//
//
//static void _shader_Init(void)
//{
//    shader_isInit = true;
//}
//
//// static srt_map _shader_Preprocess(const char *name, ShaderType type, const char *src_c)
//// {
//
////     if (src_c[0]== '\0')
////     {
////         SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Empty shader source passed.");
////         return;
////     }
//
////     // srt_string *token = ss_crefa("#type");
////     srt_string_ref raw_shaders[MAX_SHADER_TYPES];
//
////     size_t shaders_found = ss_split(
////         ss_crefa(src_c),
////         ss_crefa("#type"),
////         raw_shaders,
////         MAX_SHADER_TYPES);
//
////     srt_map *shaders = sm_alloca(SM_IS, shaders_found);
//
////     for (int i = 0; i < shaders_found; i++)
////     {
////         sm_insert_is(shaders, type, ss_ref(&raw_shaders[i]));
////     }
//// }
//
//// Load shader from file
//ShaderProgram shader_Load(ShaderType type, const char *shaderName) {
//    if(!shader_isInit) {
//        _shader_Init();
//    }
//
//    ShaderProgram   shader  = { 0 };
//    char *          path    = paths_Join(paths.assets_shaders, shaderName);
//    SDL_RWops  *    io      = SDL_RWFromFile(path, "rb");
//
//    if (io != nullptr) {
//        size_t size = (size_t)io->size(io);
//        char *buffer[size];
//
//        if(io->read(io, buffer, size, 1)) {
//            // _shader_Preprocess(type, buffer);
//            shader_LoadSrc(type, *buffer);
//        }
//
//        free(io);
//    }
//
//    free(path);
//    return shader;
//}
//
//ShaderProgram shader_LoadSrc(ShaderType type, char *source)
//{
//    if(!shader_isInit)
//    {
//        _shader_Init();
//    }
//
//    ShaderProgram   shader  = { 0 };
//
//    if (!type || !source) {
//        return shader;
//    }
//
//
//    return shader;
//}
//
////==============================================================================
//// Shader library
////==============================================================================
//
//static const unsigned int SHADERLIB_INIT_CAPACITY = 16;
//
//void shaderLib_Init(ShaderLib * lib) {
//    ReloadLib->capacity = SHADERLIB_INIT_CAPACITY;
//    ReloadLib->total = 0;
//    ReloadLib->items = (ShaderProgram **)calloc(1, sizeof(ShaderProgram *) * SHADERLIB_INIT_CAPACITY);
//
//    if (!ReloadLib->items)
//    {
//        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Error creating shader library.");
//        exit(1);
//    }
//}
//
//void shaderLib_Cleanup(ShaderLib *ReloadLib)
//{
//    free(ReloadLib->items);
//}
//
////void shaderLib_Add(ShaderLib *ReloadLib, ShaderProgram *shader) {
//    // for (int i = 0; i < ReloadLib->total; i++)
//    // {
//    //     if (shader->name == ReloadLib->items[i]->name)
//    //     {
//    //         SDL_LogError(
//    //             SDL_LOG_CATEGORY_RENDER,
//    //             "Shader %s already loaded.",
//    //             shader->name);
//    //         exit(1);
//    //     }
//    // }
//
//    // if (ReloadLib->capacity == ReloadLib->total)
//    // {
//    //     ReloadLib->capacity *= 2;
//    //     ShaderLib *items = realloc(ReloadLib->items, sizeof(ShaderLib *) * ReloadLib->capacity);
//    //     if (items)
//    //     {
//    //         ReloadLib->items = items;
//    //     }
//    // }
//
//    // ReloadLib->items[ReloadLib->total++] = shader;
////}
//
//// TODO: Implement after implementing the single shader functionality.
////void shaderLib_Load(ShaderLib *ReloadLib, const char *shaderName) {
////}
////
////// TODO: Implement after implementing the single shader functionality.
////void shaderLib_Get(ShaderLib *ReloadLib, const char *name, ShaderProgram *shader) {
////}