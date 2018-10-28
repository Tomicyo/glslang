struct HitInfo
{
	float4 ShadedColorAndHitT : SHADED_COLOR_AND_HIT_T;
};

cbuffer ViewCB : register(b0)
{
	matrix view;
	float4 viewOriginAndTanHalfFovY;
	float2 resolution;
};

RWTexture2D<float4> RTOutput				: register(u0);
RaytracingAccelerationStructure SceneBVH	: register(t0);

[shader("raygeneration")]
void RayGen() 
{
	uint2 LaunchIndex = DispatchRaysIndex().xy;
	uint2 LaunchDimensions = DispatchRaysDimensions().xy;
	uint  PixelLinearIndex = LaunchIndex.x + (LaunchIndex.y * LaunchDimensions.x);

	float2 d = (((LaunchIndex.xy + 0.5f) / resolution.xy) * 2.f - 1.f);
	float aspectRatio = (resolution.x / resolution.y);

	// Setup the ray
	RayDesc ray;
	ray.Origin = viewOriginAndTanHalfFovY.xyz;
	ray.Direction = normalize((d.x * view[0].xyz * viewOriginAndTanHalfFovY.w * aspectRatio) - (d.y * view[1].xyz * viewOriginAndTanHalfFovY.w) + view[2].xyz);
	ray.TMin = 0.1f;
	ray.TMax = 1000.f;	

	// Trace the ray
	HitInfo payload;
	payload.ShadedColorAndHitT = float4(1, 0, 0, 0);

	TraceRay(
		SceneBVH,
		RAY_FLAG_NONE,
		0xFF,
		0,
		0,
		0,
		ray,
		payload);

	RTOutput[LaunchIndex.xy] = float4(payload.ShadedColorAndHitT.rgb, 1.f);
}
