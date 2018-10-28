#include "hlsl.dxr.common.h"

// ---[ Miss Shader ]---

[shader("miss")]
void Miss(inout HitInfo payload : SV_RayPayload)
{
    payload.ShadedColorAndHitT = float4(0.2f, 0.2f, 0.2f, -1.f);
}