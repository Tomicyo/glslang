set(NGFX_COMPILER_SRCS
Source/ngfx_glslang_compiler.cpp
Source/ngfx_shader_compiler.cpp
)
add_library(ngfx_glslang SHARED ${NGFX_COMPILER_SRCS})
target_include_directories(ngfx_glslang PRIVATE Public Source)
target_link_libraries(ngfx_glslang glslang HLSL SPIRV OGLCompiler OSDependent)
target_compile_definitions(ngfx_glslang PRIVATE 
NGFX_SHADER_COMPILER_BUILD=1 
NGFX_SHADER_COMPILER_BUILD_SHARED_LIB=1)

install(TARGETS ngfx_glslang
        RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
