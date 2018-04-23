#include "./../glslang/Include/ShHandle.h"
#include "./../glslang/Include/revision.h"
#include "./../glslang/Public/ShaderLang.h"
#include "../SPIRV/GlslangToSpv.h"

#include "../StandAlone/ResourceLimits.h"

static char * TestLibHLSL = R"(
    #pragma pack_matrix( row_major )

    struct VS_IN
    {
      float3 inPos : POSITION;
      float3 inColor : COLOR;
    };

    struct VS_OUT
    {
      float4 outPos : SV_POSITION;
      float3 outColor : COLOR;
    };

    cbuffer UBO : register(b0)
    {
      row_major matrix projectMatrix;
      row_major matrix modelMatrix;
      row_major matrix viewMatrix;
    };

    [vertexshader] VS_OUT MainVS(VS_IN vsin)
    {
      VS_OUT vsout;
      vsout.outPos = projectMatrix * viewMatrix * modelMatrix * float4(vsin.inPos.xyz, 1.0);
      vsout.outColor = vsin.inColor;
      return vsout;
    }

    [pixelshader] float4 MainPS(VS_OUT psIn) : SV_TARGET {
        return float4(psIn.outColor, 1.0);
    }

    cbuffer AttactorMasses : register(b1)
    {
        float4 attractor[64];
    };
    uniform float dt;

    RWBuffer<float4> velocity_buffer : register(u2);
    RWBuffer<float4> position_buffer : register(u3);
    
    int tCost(float x)
    { return 0; }

    [numthreads(1024, 1, 1)]
    [computeshader] void MainCS(
        uint3 Gid : SV_GroupID, 
        uint3 DTid : SV_DispatchThreadID, 
        uint3 GTid : SV_GroupThreadID, 
        uint GI : SV_GroupIndex)
    {
        float4 vel = velocity_buffer[DTid.x];
        float4 pos = position_buffer[DTid.x];

        int i;

        pos.xyz += vel.xyz * dt;
        pos.w -= 0.0001 * dt;

        for (i = 0; i < 4; i++)
        {
            float3 dist = (attractor[i].xyz - pos.xyz);
            vel.xyz += dt * dt * attractor[i].w * normalize(dist) / (dot(dist, dist) + 10.0);
        }

        if (pos.w <= 0.0)
        {
            pos.xyz = -pos.xyz * 0.01;
            vel.xyz *= 0.01;
            pos.w += 1.0f;
        }
    
        position_buffer[DTid.x] = pos;
        velocity_buffer[DTid.x] = vel;
    }
    )";

using namespace glslang;

int main(int argc, const char * argv[])
{
  ShInitialize();

  EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);
    messages =
      (EShMessages)(EShMsgVulkanRules | EShMsgSpvRules | EShMsgReadHlsl);
  TProgram& program = *new TProgram;
  const char* shaderStrings[1];
  EShLanguage stage ;
  TShader* shader = new TShader(EShLangCompute);

  shaderStrings[0] = (const char*)TestLibHLSL;
  shader->setStrings(shaderStrings, 1);
  shader->setEntryPoint("MainCS");

  if (!shader->parse(&DefaultTBuiltInResource, 100, false, messages))
  {
    puts(shader->getInfoLog());
    puts(shader->getInfoDebugLog());
  }

  int i = shader->getNumDeclEntryPoints();
  const char *name = shader->getDeclEntryName(1);
  EShLanguage lang = shader->getDeclEntryStage(name);
  program.addShader(shader);

  if (!program.link(messages))
  {
    puts(program.getInfoLog());
    puts(program.getInfoDebugLog());
  }

  if (program.buildReflection())
  {
    //GlslangToSpv(*program.getIntermediate(stage), );
  }

  ShFinalize();
  return 0;
}