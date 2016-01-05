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
#include <cmath>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include "gl_commands.hpp"

#include <libpng-1.6.9/png.h>
#include <libpng-1.6.9/pngstruct.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_precision.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <pugixml/pugixml.hpp>

#include <box2d/Box2D.h>

#define string_shader(__shader__)  #__shader__
#include "built_in_shaders.h"

typedef signed char i8;
typedef unsigned char ui8;
typedef signed short i16;
typedef unsigned short ui16;
typedef signed int i32;
typedef unsigned int ui32;
typedef long long i64;
typedef unsigned long long ui64;
typedef float f32;

#define TO_RGB565(r, g, b) (unsigned short) (((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3))
#define TO_RGBA4444(r, g, b, a) (unsigned short) ((r >> 4) << 12 | (g >> 4) << 8 | (b >> 4) << 4 | (a >> 4))

#endif
