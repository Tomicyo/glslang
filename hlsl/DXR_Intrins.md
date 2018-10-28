|Intrinsic\shader|ray generation|intersection|any hit|closest hit|miss|callable|
|:---|:---:|:---:|:---:|:---:|:---:|:---:|
|CallShader(Id, T)                          |X| | |X|X|X|
|TraceRay(AS,Flag)                          |X| | |X|X| |
|ReportHit(Hit, HitKind, Attrib)            | |X| | | | |
|IgnoreHit()                                | | |X| | | |
|AcceptHitAndEndSearch()                    | | |X| | | |
| uint2 DispatchRaysIndex()					|X|X|X|X|X|X|
| uint2 DispatchRaysDimensions()			|X|X|X|X|X|X|
| float3 WorldRayOrigin()					| |X|X|X|X| |
| float3 WorldRayDirection()				| |X|X|X|X| |
| float RayTMin()							| |X|X|X|X| |
| float RayTCurrent()						| |X|X|X|X| |
| uint RayFlags()							| |X|X|X|X| |
| uint InstanceIndex()						| |X|X|X| | |
| uint InstanceID()							| |X|X|X| | |
| uint PrimitiveIndex()						| |X|X|X| | |
| float3 ObjectRayOrigin()					| |X|X|X| | |
| float3 ObjectRayDirection()				| |X|X|X| | |
| float3x4 ObjectToWorld()					| |X|X|X| | |
| float3x4 WorldToObject()					| |X|X|X| | |
| uint HitKind()							| | |X|X| | |