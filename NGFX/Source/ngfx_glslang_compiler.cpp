#include "../../SPIRV/GlslangToSpv.h"
#include "ngfx_shader_compiler_private.h"
#include "StandAlone/ResourceLimits.h"
#if ENABLE_SPIRV_CROSS
#include <spirv_glsl.hpp>
#include <spirv_msl.hpp>
#endif
namespace ngfx {
using namespace glslang;

class GLSlangIncluder : public TShader::Includer {
public:
    GLSlangIncluder(IIncluder *pIncluder) : m_Includer(pIncluder) {}

    virtual IncludeResult *includeSystem(const char *headerName, const char *includerName,
                                         size_t /*inclusionDepth*/) override
    {
        if (!m_Includer)
        {
            return nullptr;
        }
        char *pContent = nullptr;
        uint64_t length = 0;
        Result ret =
            m_Includer->Open(IncludeType::System, headerName, includerName, (void **)&pContent, &length, nullptr);
        return ret == Result::Ok ? new IncludeResult(headerName, pContent, length, pContent) : nullptr;
    }

    // For the "local"-only aspect of a "" include. Should not search in the
    // "system" paths, because on returning a failure, the parser will
    // call includeSystem() to look in the "system" locations.
    IncludeResult *includeLocal(const char *headerName, const char *includerName, size_t /*inclusionDepth*/) override
    {
        if (!m_Includer)
        {
            return nullptr;
        }
        char *pContent = nullptr;
        uint64_t length = 0;
        Result ret =
            m_Includer->Open(IncludeType::Local, headerName, includerName, (void **)&pContent, &length, nullptr);
        return ret == Result::Ok ? new IncludeResult(headerName, pContent, length, pContent) : nullptr;
    }

    void releaseInclude(IncludeResult *result) override
    {
        if (!m_Includer)
            return;
        m_Includer->Close(result->userData);
        delete result;
    }

    ~GLSlangIncluder() override {}

private:
    IIncluder *m_Includer;
};

class SpirvBlob : public IBlob {
public:
    SpirvBlob(std::vector<uint32_t> &&InSpirv);
    ~SpirvBlob() override {}
    const void *GetData() override;
    uint64_t GetSize() override;

private:
    std::vector<uint32_t> m_Spirv;
};

class StringBlob : public IBlob {
public:
    StringBlob(std::string &&InStr) : m_Str(std::forward<std::string>(InStr)) {}
    ~StringBlob() override {}
    const void *GetData() override { return m_Str.data(); }
    uint64_t GetSize() override { return m_Str.length(); }

private:
    std::string m_Str;
};

class GlslCompilerResult : public ICompilerResult {
public:
    GlslCompilerResult(IBlob *pResult = nullptr, IBlob *pErr = nullptr) 
        : m_Result(pResult), m_Err(pErr) {}

    ~GlslCompilerResult() override
    {
        if (m_Err)
        {
            m_Err->Release();
            m_Err = nullptr;
        }
        if (m_Result)
        {
            m_Result->Release();
            m_Result = nullptr;
        }
    }

    void SetResult(IBlob *pBlob) {}
    void SetError(IBlob *pErr) {}

    Result GetStatus() override { return m_Status; }
    Result GetResult(IBlob **pResult) override
    {
        if (!pResult)
        {
            return Result::ParamError;
        }
        *pResult = m_Result;
        return Result::Ok;
    }
    Result GetError(IBlob **pErr) override
    {
        if (!pErr)
        {
            return Result::ParamError;
        }
        *pErr = m_Err;
        return Result::Ok;
    }

private:
    IBlob *m_Result;
    IBlob *m_Err;
    Result m_Status;
};

EShLanguage Convert(ShaderStageBit Shader)
{
    switch (Shader)
    {
    case ShaderStageBit::Vertex:
        return EShLangVertex;
    case ShaderStageBit::Fragment:
        return EShLangFragment;
    case ShaderStageBit::Compute:
        return EShLangCompute;
    case ShaderStageBit::Geometry:
        return EShLangGeometry;
    case ShaderStageBit::TessailationEval:
        return EShLangTessEvaluation;
    case ShaderStageBit::TessailationControl:
        return EShLangTessControl;
    }
    return EShLangCount;
}

GlslangCompiler::GlslangCompiler(GlslangCompiler::Output O) : m_OutputType(O) {}

GlslangCompiler::~GlslangCompiler() {}

class GlslangScope {
public:
    GlslangScope() { ShInitialize(); }
    ~GlslangScope() { ShFinalize(); }
};

Result GlslangCompiler::Compile(const char *Source, const char *File, const char *EntryPoint, ShaderProfile Profile,
                                ShaderStageBit ShaderType, ShaderOptimizeLevel OptLevel, IIncluder *pIncluder,
                                const ShaderDefinition *Definitions, ICompilerResult **ppResult)
{
    if (!ppResult)
        return Result::ParamError;
    GlslangScope scope;
    GLSlangIncluder includer(pIncluder);
    EShMessages message = EShMessages(EShMsgVulkanRules | EShMsgSpvRules | EShMsgReadHlsl);
    EShLanguage language = Convert(ShaderType);
    auto shader = std::make_unique<TShader>(language);
    int len = (int)strlen(Source);
    shader->setStringsWithLengthsAndNames(&Source, &len, &File, 1);
    shader->setEntryPoint(EntryPoint);
    //shader->setEnvInput(glslang::EShSourceHlsl);
    std::string defines;
    if (Definitions)
    {
        const ShaderDefinition *defs = Definitions;
        while (defs)
        {
            if (defs->Name)
            {
                defines += std::string("#define ") + defs->Name;
            } else
            {
                break;
            }
            if (defs->Definition)
            {
                defines += std::string("=") + defs->Name;
            }
            defines += "\n";
            defs++;
        }
    }
    shader->setPreamble(defines.c_str());
    if (!shader->parse(&DefaultTBuiltInResource, 100, false, message, includer))
    {
        std::string info = shader->getInfoLog();
        info += "\n";
        info += shader->getInfoDebugLog();
        *ppResult = new GlslCompilerResult(nullptr, new StringBlob(std::move(info)));
        return Result::Failed;
    } else
    {
        auto program = std::make_unique<TProgram>();
        program->addShader(shader.get());
        if (!program->link(message))
        {
            std::string info = program->getInfoLog();
            info += "\n";
            info += program->getInfoDebugLog();
            
            *ppResult = new GlslCompilerResult(nullptr, new StringBlob(std::move(info)));
            return Result::Failed;
        } else
        {
            if (program->buildReflection())
            {
            }
            std::vector<unsigned int> spirv;
            GlslangToSpv(*program->getIntermediate(language), spirv);

            switch (m_OutputType)
            {
            case CO_SPIRV:
            {
                *ppResult = new GlslCompilerResult(new SpirvBlob(std::move(spirv)));
                break;
            }
#if ENABLE_SPIRV_CROSS
                case CO_GLSL: // SPIRV->GLSL
                {
                    auto glsl_compiler = std::make_unique<spirv_cross::CompilerGLSL>(spirv.data(), spirv.size());
                    spirv_cross::CompilerGLSL::Options options;
                    options.version = 450;
                    options.es = false;
                    options.vertex.fixup_clipspace = false;
                    glsl_compiler->set_options(options);
                    *ShaderOutput = new StringBlob(std::move(glsl_compiler->compile()));
                    break;
                }
                case CO_ESSL: // SPIRV->ESSL
                {
                    auto glsl_compiler = std::make_unique<spirv_cross::CompilerGLSL>(spirv.data(), spirv.size());
                    spirv_cross::CompilerGLSL::Options options;
                    options.version = 310;
                    options.es = true;
                    options.vertex.fixup_clipspace = false;
                    glsl_compiler->set_options(options);
                    *ShaderOutput = new StringBlob(std::move(glsl_compiler->compile()));
                    break;
                }
                case CO_MSL: // SPIRV->MSL
                {
                    auto msl_compiler = std::make_unique<spirv_cross::CompilerMSL>(spirv.data(), spirv.size());
                    spirv_cross::CompilerMSL::Options options;
                    msl_compiler->set_options(options);
                    *ShaderOutput = new StringBlob(std::move(msl_compiler->compile()));
                    break;
                }
#endif
                }
            
            return Result::Ok;
        }
    }
    return Result::Failed;
}

Result GlslangCompiler::PreProcess(const char *strSource, const char *strFile, const char *strEntryPoint,
                                   ShaderProfile Profile, ShaderStageBit ShaderType, ShaderOptimizeLevel OptLevel,
                                   IIncluder *pIncluder, const ShaderDefinition *Definitions, ICompilerResult **pResult)
{
    return Result::Failed;
}

SpirvBlob::SpirvBlob(std::vector<uint32_t> &&InSpirv) : m_Spirv(std::forward<std::vector<uint32_t>>(InSpirv)) {}
const void *SpirvBlob::GetData() { return m_Spirv.data(); }
uint64_t SpirvBlob::GetSize() { return m_Spirv.size() * sizeof(uint32_t); }
} // namespace ngfx