#pragma once

#define NS_GAF_BEGIN    namespace gaf {
#define NS_GAF_END      }
#define USING_NS_GAF    using namespace gaf


#define GAF_SAFE_RELEASE_MAP(__type, __instance)\
for (__type::iterator i = __instance.begin(), e = __instance.end(); i != e; ++i) {\
    i->second->release(); \
} __instance.clear()

#define GAF_SAFE_RELEASE_ARRAY(__type, __instance)\
for (__type::iterator i = __instance.begin(), e = __instance.end(); i != e; ++i) {\
    (*i)->release(); \
} __instance.clear()

#define GAF_SAFE_RELEASE_ARRAY_WITH_NULL_CHECK(__type, __instance)\
for (__type::iterator i = __instance.begin(), e = __instance.end(); i != e; ++i) {\
if (*i) \
    (*i)->release(); \
} __instance.clear()

#define GAF_RELEASE_ARRAY(__type, __instance)\
for (__type::iterator i = __instance.begin(), e = __instance.end(); i != e; ++i) {\
    delete *i; \
} __instance.clear()

#define GAF_RELEASE_MAP(__type, __instance)\
for (__type::iterator i = __instance.begin(), e = __instance.end(); i != e; ++i) {\
    delete i->second; \
} __instance.clear()


#ifndef GAF_ENABLE_NEW_UNIFORM_SETTER
// Fast uniform setter is available since v3.2
#define GAF_ENABLE_NEW_UNIFORM_SETTER COCOS2D_VERSION >= 0x00030200
#endif

// Disable this option and call GAFShaderManager::Initialize by you own
// anywhere you want. These could be useful when you pass GAF loading into background thread
// whereas GAFShaderManager::Initialize is allowed to be called only on OpenGL's thread

#define GAF_ENABLE_SHADER_MANAGER_AUTOMATIC_INITIALIZATION 0

#ifndef GAF_ENABLE_SHADER_MANAGER_AUTOMATIC_INITIALIZATION
#define     GAF_ENABLE_SHADER_MANAGER_AUTOMATIC_INITIALIZATION 1
#endif

#define CHECK_CTX_IDENTITY 1
