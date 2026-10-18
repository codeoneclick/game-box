#pragma once
#include <algorithm>
namespace game {
inline float terrain_editor_toolbar_scale(float width,float height){
 // Reserve the tools panel at its actual height-dependent scale and a gutter.
 const float right_scale=std::max(.65f,std::min(1.f,(height-390.f)/398.f));
 return std::min(1.f,std::max(1.f,width-48.f-386.f*right_scale)/900.f);
}
}
