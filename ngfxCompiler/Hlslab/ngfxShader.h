#pragma once

#ifndef __ngfx_shader_compiler__
#define __ngfx_shader_compiler__
/*
Shader "NGFXShader/Sample" {
    Parameters {
        BaseColor("Base Color For X", vec4) = (1.0)
        Smoothness("Smoothness for x", tex2d) = "white" {}
    }
    Code {
        BEGIN_HLSL
        // HLSL Code
        END_HLSL
    }
    Permutate {
        USE_PBR = 0|1,
        SHADER_LEVEL = 0|1|2,
        SHADOW_QUALITY = 0|1|2,
        MATERIAL_QUALITY = 0|1|2,
        USE_HDR = 0|1
    }
    SubPass {
        DepthWrite ON,
        ColorWrite OFF,
        StencilWrite ON,
        DepthCompare GreatEqual,
        Defines {
            USE_PBR = 0,
            USE_ALPHA_TEST = 1
        }
    }
    SubPass "Depth Prepass" {
        DepthWrite OFF,
        ColorWrite OFF,
        StencilWrite OFF,
        DepthCompare LessEqual,
        Defines {
            USE_PBR = 1,
            USE_ALPHA_TEST = 0
        }
    }
    Pipeline {
        VertexLayout,
        RenderState,
        Shader,
        Attachment,
        SubPasses
    }
}
*/
namespace ngfx {

}

#endif