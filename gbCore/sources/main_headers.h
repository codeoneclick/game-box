//
//  mainHeaders.h
//  gbCore
//
//  Created by sergey.sergeev on 8/11/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef main_headers_h
#define main_headers_h

#include <map>
#include <set>
#include <list>
#include <forward_list>
#include <queue>
#include <string>
#include <vector>
#include <array>
#include <bitset>
#include <numeric>
#include <future>
#include <algorithm>
#include <mutex>
#include <thread>
#include <fstream>
#include <strstream>
#include <sstream>
#include <iostream>
#include <assert.h>
#include <functional>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <ctime>
#include <iomanip>

#define _USE_MATH_DEFINES
#include <cmath>

typedef signed char i8;
typedef unsigned char ui8;
typedef signed short i16;
typedef unsigned short ui16;
typedef signed int i32;
typedef unsigned int ui32;
typedef long long i64;
typedef unsigned long long ui64;
typedef float f32;
typedef double f64;

#if defined(__OSX__) || defined(__IOS__) || defined(__TVOS__)

#include <sys/mman.h>
#include <unistd.h>

#endif

#if defined(__WINOS__)

#include <windows.h>

#endif

#define NO_GRAPHICS_API -1
#define OPENGL_20_API 1
#define OPENGL_30_API 2
#define VULKAN_API 3

#include "gl_commands.hpp"

#include <libpng-1.6.23/png.h>
#include <libpng-1.6.23/pngstruct.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_precision.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <pugixml/pugixml.hpp>
#include <freetype-gl/texture-font.h>

#include <box2d/Box2D.h>

#include <jsoncpp-1.7.2/json/json.h>

#define string_shader(__shader__)  #__shader__
#include "built_in_shaders.h"

#define TO_RGB565(r, g, b) (unsigned short) (((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3))
#define TO_RGBA4444(r, g, b, a) (unsigned short) ((r >> 4) << 12 | (g >> 4) << 8 | (b >> 4) << 4 | (a >> 4))


#if defined(__OSX__) || defined(__IOS__) || defined(__TVOS__)

#define __attr_unused __attribute__((__unused__))

#elif defined(__WINOS__)

#define __attr_unused 

#endif

using ctti_guid_t = uintptr_t;
#define CTTI_CLASS_GUID(__class__, __guids_container__, ...) \
using self_t = __class__; \
static ctti_guid_t class_guid() \
{ \
static ctti_guid_t guid = 0; \
static std::once_flag cached_classes_guids; \
std::call_once(cached_classes_guids, [] { \
__guids_container__.insert(reinterpret_cast<ctti_guid_t>(&class_guid)); \
guid = static_cast<ctti_guid_t>(__guids_container__.size()); \
});\
return guid;\
}\
virtual ctti_guid_t instance_guid() __VA_ARGS__\
{\
return __class__::class_guid();\
}\

#define PROP(__directive__, __class__, __prop__, __type__, ...) \
std::shared_ptr<gb::binding_property<__type__>> __prop__ = std::make_shared<gb::binding_property<__type__>>(__VA_ARGS__); \
static std::string PROPERTY_ID_##__prop__() \
{ \
	return #__prop__; \
} \

#define PROP_DECL_R(__prop__, ...) \
typename decltype(self_t::record_t::__prop__)::element_type::value_t get_##__prop__() const { \
	return this->record_data->get()->__prop__->get(); \
} \

#define PROP_DECL_W(__prop__, ...) \
void set_##__prop__(const typename decltype(self_t::record_t::__prop__)::element_type::value_t& value) { \
	this->record_data->get()->__prop__->set(value); \
} \

#endif
