#include "voxel_tree_builder.h"
#include "terrain_editor_state.h"
#include <cassert>
#include <iostream>
int main(){for(int i=0;i<5;++i){size_t maxtris=0;for(unsigned s=0;s<128;++s){game::EditorTree t;t.type=static_cast<game::TreeType>(i);t.id=s*13;auto m=game::build_voxel_tree(t,s);assert(m.vertices.size()<65535);assert(m.indices.size()/3<2000);maxtris=std::max(maxtris,m.indices.size()/3);}std::cout<<i<<" max triangles="<<maxtris<<std::endl;}}
