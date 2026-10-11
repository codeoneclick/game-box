#define main_headers_h
#define declarations_h
#define resource_serializer_h
#define VULKAN_API 2
#define USED_GRAPHICS_API 2
#define OPENGL_20_API 0
#define OPENGL_30_API 1
#include <cstdint>
#include <memory>
#include <map>
#include <string>
#include <vector>
#include <mutex>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <vulkan/vulkan.h>
#include <shaderc/shaderc.hpp>
using i32=int32_t;using ui32=uint32_t;using ui64=uint64_t;using f32=float;
namespace gb {class window_impl;namespace gl {struct constant{static const ui32 vertex_shader=0x8B31,fragment_shader=0x8B30;};}}
#include "../gbCore/sources/shader_compiler_glsl.cpp"
// CPU preparation must never ask for a native device.
namespace gb {std::shared_ptr<vk_device> vk_device::get_instance(){throw std::runtime_error("CPU precompile accessed device");}VkDevice vk_device::get_logical_device(){throw std::runtime_error("CPU precompile accessed device");}}
static void require(bool ok,const char* why){if(!ok)throw std::runtime_error(why);}
int main(){try{
 const std::string source="uniform vec3 tint;\nuniform float weights[2];\nvoid main(){gl_Position=vec4(tint*weights[0],1.0);}\n";
 std::vector<gb::shader_custom_uniform_desc> a,b;
 const auto converted=gb::shader_compiler_glsl::convert_to_vulkan_source(source,11,&a);
 require(converted==gb::shader_compiler_glsl::convert_to_vulkan_source(source,11,&b),"cache changes converted source");
 require(a.size()==2&&b.size()==2&&a[1].m_offset==16&&b[1].m_stride==16,"cached reflection changes std140 layout");
 require(gb::shader_compiler_glsl::convert_to_vulkan_source(source,12).find("binding = 12")!=std::string::npos,"conversion cache lost binding key");
 std::string message;require(gb::shader_compiler_glsl::precompile(source,gb::gl::constant::vertex_shader,&message),message.c_str());
 require(!gb::shader_compiler_glsl::precompile(source,gb::gl::constant::fragment_shader,&message),"compiled vertex entry as fragment stage");
 const std::string simple="#version 450\nvoid main(){gl_Position=vec4(0.0);}\n";
 auto one=gb::compile_spirv(simple,gb::gl::constant::vertex_shader,&message),two=gb::compile_spirv(simple,gb::gl::constant::vertex_shader,&message);
 require(one&&one==two,"SPIR-V cache missed identical assembled source");
 shaderc::Compiler compiler;const auto direct=compiler.CompileGlslToSpv(simple,shaderc_glsl_vertex_shader,"shader");
 require(direct.GetCompilationStatus()==shaderc_compilation_status_success&&std::vector<ui32>(direct.cbegin(),direct.cend())==*one,"cached compilation changes SPIR-V bits");
 require(!gb::compile_spirv(simple+"invalid token",gb::gl::constant::vertex_shader,&message),"source mutation reused stale SPIR-V");
 std::cout<<"PASS CPU shader preparation, exact SPIR-V, stage/source/binding cache keys and reflection\n";return 0;
 }catch(const std::exception& e){std::cerr<<"FAIL "<<e.what()<<"\n";return 1;}}
