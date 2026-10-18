#pragma once
#include "main_headers.h"
#include "declarations.h"
#include "terrain_lod.h"
#include "ces_sun_shadow_system.h"
#include <functional>
namespace game {
class TerrainLodRenderer {
 struct implementation;std::shared_ptr<implementation> impl_;
public:
 using Upload=std::function<gb::mesh_3d_shared_ptr(const TerrainLodVariant&,const std::string&)>;
 // Kind: 0 main image, 1 stable water source (no material), 2 water mask.
 using Create=std::function<gb::shape_3d_shared_ptr(const std::string&,int)>;
 using Destroy=std::function<void(const gb::shape_3d_shared_ptr&)>;
 using Interaction=std::function<void(const gb::shape_3d_shared_ptr&,const gb::shape_3d_shared_ptr&)>;
 using Retire=std::function<void(const gb::mesh_3d_shared_ptr&)>;
 using Casters=std::function<void(const std::vector<TerrainShadowCaster>&)>;
 TerrainLodRenderer(Upload,Create,Destroy,Interaction,Retire,Casters);
 void stage(const std::string&,std::shared_ptr<const TerrainLodGeneration>);
 bool publish_ready(const TerrainLodVersion&);
 void update_main(const TerrainLodView&,bool wireframe=false,int debug_mode=0);
 std::string diagnostics()const;
 size_t selected_triangles()const;
};
}
