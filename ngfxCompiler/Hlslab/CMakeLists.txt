set(ngfxShader_SRC
ngfxShader.cpp
ngfxShader.h
ngfxShader_p.h
ngfxShaderBinary.cpp
ngfxShaderParser.cpp
LibCompiler.cpp
)
add_library(ngfxShaderCompiler SHARED ${ngfxShader_SRC})
set_property(TARGET ngfxShaderCompiler PROPERTY FOLDER tools)
set_property(TARGET ngfxShaderCompiler PROPERTY POSITION_INDEPENDENT_CODE ON)
glslang_set_link_args(ngfxShaderCompiler)