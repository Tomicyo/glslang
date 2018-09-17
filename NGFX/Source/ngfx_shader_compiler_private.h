#ifndef __ngfx_shader_compiler_private__
#define __ngfx_shader_compiler_private__

#include "ngfx_shader_compiler.h"

namespace ngfx {

class GlslangCompiler : public IShaderCompiler {
public:
    enum Output {
        CO_SPIRV,
        CO_GLSL,
        CO_ESSL,
        CO_MSL, // Metal Source
    };

    GlslangCompiler(Output O);
    virtual ~GlslangCompiler() override;

    virtual Result Compile(const char *Source, const char *File, const char *EntryPoint, ShaderProfile Profile,
                           ShaderStageBit ShaderType, ShaderOptimizeLevel OptLevel, IIncluder *pIncluder,
                           const ShaderDefinition *Definitions, ICompilerResult**ppResult) override;

    virtual Result PreProcess(const char *strSource, const char *strFile, const char *strEntryPoint,
                              ShaderProfile Profile, ShaderStageBit ShaderType, ShaderOptimizeLevel OptLevel,
                              IIncluder *pIncluder, const ShaderDefinition *Definitions, ICompilerResult **pResult) override;

private:
    Output m_OutputType;
};

} // namespace ngfx

#endif