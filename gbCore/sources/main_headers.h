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

#if defined(__WIN32__)

#include <windows.h>

#endif

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

#elif defined(__WIN32__)

#define __attr_unused 

#endif

#define CTTI_CLASS_GUID(__class__, __guids_container__, ...) \
static uint8_t class_guid() \
{ \
static uint8_t guid = 0; \
static std::once_flag cached_classes_guids; \
std::call_once(cached_classes_guids, [] { \
__guids_container__.insert(reinterpret_cast<uintptr_t>(&class_guid)); \
guid = __guids_container__.size(); \
});\
return guid;\
}\
virtual uint8_t instance_guid() __VA_ARGS__\
{\
return __class__::class_guid();\
}\

#endif
