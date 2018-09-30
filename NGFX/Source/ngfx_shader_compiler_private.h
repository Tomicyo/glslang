#ifndef __ngfx_shader_compiler_private__
#define __ngfx_shader_compiler_private__

#include "ngfx_shader_compiler.h"

namespace ngfx {

class GlslangCompiler : public IShaderCompiler {
public:

    GlslangCompiler();
    virtual ~GlslangCompiler() override;

    virtual void SetDeviceLimits(DeviceLimits const &limits);
    virtual void SetOutputType(CompileOutputType const &oType) { m_OutputType = oType; }

    virtual Result Compile(const char *Source, const char *File, CompileOptions const &options, IIncluder *pIncluder,
                           const ShaderDefinition *Definitions, ICompilerResult**ppResult) override;
/*
    virtual Result PreProcess(const char *strSource, const char *strFile, const char *strEntryPoint,
                              ShaderProfile Profile, ShaderStageBit ShaderType, ShaderOptimizeLevel OptLevel,
                              IIncluder *pIncluder, const ShaderDefinition *Definitions, ICompilerResult **pResult) override;*/

private:
    CompileOutputType m_OutputType;
    DeviceLimits m_Limits;
};

} // namespace ngfx

#endif