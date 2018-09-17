#include "ngfx_shader_compiler_private.h"
#include <memory>

using namespace ngfx;

ngfx::Result ngfxCreateGlslangCompiler(ngfx::IShaderCompiler**ppCompiler) { 
    *ppCompiler = new GlslangCompiler(GlslangCompiler::CO_SPIRV);
    return Result::Failed; 
}

ngfx::Result ngfxDestroyGlslangCompiler(ngfx::IShaderCompiler* pCompiler)
{
    delete pCompiler;
    return Result::Failed; 
}