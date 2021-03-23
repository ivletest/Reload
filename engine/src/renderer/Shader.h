#ifndef __SHADER_H
#define __SHADER_H


#include "../Common.h"
#include "../lib/containers/List.h"

//==============================================================================
// Enums
//==============================================================================
// enum UniformType
// {
//     None = 0,
//     Float,
//     Float2,
//     Float3,
//     Float4,
//     Matrix3x3,
//     Matrix4x4,
//     Int32,
//     Uint32
// };

typedef enum shader_type {
    FragmentShader,
    FragmentShaderArb,
    VertexShader,
    VertexShaderArb,
    GeometryShader,
    TessEvaluationShader,
    TessControlShader,
    ComputeShader
} ShaderType;

typedef enum shader_domain {
    VERTEX,
    PIXEL
} ShaderDomain;

// TODO: Allign the structs.

//==============================================================================
// Shader uniform declaration
//==============================================================================
typedef struct {
    uint32_t size;
    uint32_t count;
    uint32_t offset;
    ShaderDomain shaderDomain;
    std::string *name;
} ShaderUniform;

typedef List<ShaderUniform> ShaderUniformList;

ShaderUniform *ShaderUniform_new(const char *name);
void ShaderUniform_free(ShaderUniform *target);

//==============================================================================
// Shader uniform buffer declaration
//==============================================================================
typedef struct {
    uint32_t register_;
    uint32_t size;
    ShaderUniformList *uniformDecls;
    std::string *name;
} ShaderUniformBuffer;

typedef List<ShaderUniformBuffer> ShaderUniformBufferList;

ShaderUniformBuffer *ShaderUniformBuffer_new();
void ShaderUniformBuffer_free(ShaderUniformBuffer *target);
ShaderUniform *shaderUniformBuffer_FindUniformDecl(const char *name);

//==============================================================================
// Shader struct
//==============================================================================
typedef struct {
    uint32_t size;
    uint32_t offset;
    std::string *name;
    ShaderUniformList *fields;
} ShaderStruct;

ShaderStruct *ShaderStruct_new(const char *name);
void ShaderStruct_free(ShaderStruct *target);
void ShaderStruct_addField(ShaderUniform *field);

typedef List<ShaderStruct> ShaderStructList;

//==============================================================================
// Shader resource
//==============================================================================
typedef struct {
    uint32_t register_;
    uint32_t size;
    std::string *name;
} ShaderResource;

typedef List<ShaderResource> ShaderResourceList;

//==============================================================================
// Shader program
//==============================================================================
typedef struct {
    int programId;
    int compute;
    int vertex;
    int tessCtrl;
    int tessEval;
    int geometry;
    int fragment;
} ShaderProgram;

//==============================================================================
// Shader library
//==============================================================================
typedef struct {
    int capacity;
    int total;
    ShaderProgram **items;
} ShaderLib;

//==============================================================================
// Shader
//==============================================================================

//extern srt_map *shader_Types;

// Functions for loading/unloading shader either from source file or a source
// string to/from GPU memory (VRAM) and binding to the default locations.
ShaderProgram shader_Load(ShaderType type, const char *shaderName);
ShaderProgram shader_LoadSrc(ShaderType type, char *source);
void shader_Unload(ShaderProgram shader);

unsigned int shader_Compile(const char *shaderStr, int type);                    // Compile custom shader and return shader id
unsigned int shader_LoadProgram(unsigned int vShaderId, unsigned int fShaderId); // Load custom shader program

// Shader configuration functions
// extern int    shader_GetLocation(Shader shader, const char *uniformName);      // Get shader uniform location
// extern int    shader_GetLocationAttrib(Shader shader, const char *attribName); // Get shader attribute location
// extern void   shader_SetValue(Shader shader, int uniformLoc, const void *value, int uniformType);               // Set shader uniform value
// extern void   shader_SetValueV(Shader shader, int uniformLoc, const void *value, int uniformType, int count);   // Set shader uniform value vector
// extern void   shader_SetValueMatrix(Shader shader, int uniformLoc, Matrix mat);         // Set shader uniform value (matrix 4x4)
// extern void   shader_SetValueTexture(Shader shader, int uniformLoc, Texture2D texture); // Set shader uniform value for texture
// extern void   shader_SetMatrixProjection(Matrix proj);                              // Set a custom projection matrix (replaces internal projection matrix)
// extern void   shader_SetMatrixModelview(Matrix view);                               // Set a custom modelview matrix (replaces internal modelview matrix)
// extern Matrix shader_GetMatrixModelview(void);                                    // Get internal modelview matrix
// extern Matrix shader_GetMatrixProjection(void);                                   // Get internal projection matrix

//==============================================================================
// Shader library
//==============================================================================
// Initialize the shader librarary
void shaderLib_Init(ShaderLib *lib);
// Free the resources from RAM
void shaderLib_Cleanup(ShaderLib *lib);
// Add new shader to the library
//void shaderLib_Add(ShaderLib *lib, ShaderProgram *shader);
//
//void shaderLib_Load(ShaderLib *lib, const char *shaderName);
//
//void shaderLib_Get(ShaderLib *lib, const char *name, ShaderProgram *shader);

//==============================================================================
// Utils
//==============================================================================

#endif // !__SHADER_H