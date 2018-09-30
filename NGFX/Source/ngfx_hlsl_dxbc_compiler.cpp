#include "ngfx_shader_compiler_private.h"
#if _WIN32
#include <wrl/client.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
using Microsoft::WRL::ComPtr;
namespace ngfx {
}
#endif
