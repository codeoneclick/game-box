#include "terrain_editor_render_cache.h"
#include "terrain_generator.h"
#include "terrain_mesh_builder.h"
#include "terrain_mesh_optimizer.h"
#include "terrain_lod.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>

// Read-only CPU geometry audit. No renderer, level file or source asset changes.
namespace {
using namespace game;
using Clock = std::chrono::steady_clock;
struct Counts {
    size_t meshes=0,vertices=0,indices=0,unique=0,top=0,wall=0,dry_top=0,dry_wall=0;
    size_t max_vertices=0;
    size_t planar_interior_vertices=0;
    double top_area=0;
    void add(const terrain_mesh_data& mesh,bool deduplicate=true) {
        if(mesh.indices.empty()) return;
        ++meshes; vertices+=mesh.vertices.size(); indices+=mesh.indices.size();
        max_vertices=std::max(max_vertices,mesh.vertices.size());
        // Exact float bits and every vertex attribute: this is a conservative
        // sharing opportunity, not a position-only weld across material seams.
        std::set<std::array<uint32_t,17>> keys;
        std::vector<std::array<uint32_t,17>> attribute_keys;
        std::vector<size_t> remap;
        std::map<std::array<uint32_t,17>,size_t> ids;
        if(deduplicate) for(const auto& v:mesh.vertices) {
            std::array<uint32_t,17> key{}; size_t i=0;
            auto scalar=[&](float value){std::memcpy(&key[i++],&value,sizeof(value));};
            for(float f:v.position)scalar(f);for(float f:v.normal)scalar(f);
            for(int c=0;c<4;++c)key[i]|=uint32_t(v.color[c])<<(c*8);++i;
            for(float f:v.texcoord)scalar(f);for(float f:v.grass_bend)scalar(f);
            for(float f:v.tree_bend)scalar(f);for(float f:v.editor_palette)scalar(f);
            key[i++]=v.outline_id;key[i++]=v.stone_texture?1:0;
            if(i!=key.size())throw std::runtime_error("Audit attribute key size mismatch");
            keys.insert(key);
            auto inserted=ids.emplace(key,ids.size());remap.push_back(inserted.first->second);
            key[0]=key[2]=0;attribute_keys.push_back(key);
        }
        unique+=deduplicate?keys.size():mesh.vertices.size();
        struct Patch {std::set<size_t> vertices;std::map<std::pair<size_t,size_t>,size_t> edges;};
        std::map<std::array<uint32_t,17>,Patch> patches;
        for(size_t i=0;i+2<mesh.indices.size();i+=3) {
            const auto& a=mesh.vertices.at(mesh.indices[i]);
            const auto& b=mesh.vertices.at(mesh.indices[i+1]);
            const auto& c=mesh.vertices.at(mesh.indices[i+2]);
            if(a.normal[1]>.99f&&b.normal[1]>.99f&&c.normal[1]>.99f) {
                ++top;if(a.position[1]>0)++dry_top;
                top_area+=std::abs(double(b.position[0]-a.position[0])*(c.position[2]-a.position[2])
                    -double(b.position[2]-a.position[2])*(c.position[0]-a.position[0]))*.5;
                if(deduplicate) {
                    const auto ia=mesh.indices[i],ib=mesh.indices[i+1],ic=mesh.indices[i+2];
                    if(attribute_keys[ia]==attribute_keys[ib]&&attribute_keys[ia]==attribute_keys[ic]) {
                        auto& patch=patches[attribute_keys[ia]];
                        const size_t tri[]={remap[ia],remap[ib],remap[ic]};
                        for(int j=0;j<3;++j){patch.vertices.insert(tri[j]);++patch.edges[std::minmax(tri[j],tri[(j+1)%3])];}
                    }
                }
            } else {++wall;if(std::max({a.position[1],b.position[1],c.position[1]})>0)++dry_wall;}
        }
        for(const auto& entry:patches) {
            std::set<size_t> boundary;bool manifold=true;
            for(const auto& edge:entry.second.edges) {
                if(edge.second==1){boundary.insert(edge.first.first);boundary.insert(edge.first.second);}
                if(edge.second>2)manifold=false;
            }
            if(manifold)planar_interior_vertices+=entry.second.vertices.size()-boundary.size();
        }
    }
    void print(const char* name) const {
        std::cout<<name<<": meshes="<<meshes<<" vertices="<<vertices<<" triangles="<<indices/3
            <<" top="<<top<<" wall="<<wall<<" dry_top="<<dry_top<<" dry_wall="<<dry_wall
            <<" exact_unique_per_mesh="<<unique<<" max_mesh_vertices="<<max_vertices
            <<" top_area_m2="<<top_area
            <<" cpu_payload_MiB="<<(vertices*sizeof(terrain_vertex)+indices*2)/1048576.
            <<" gpu_payload_MiB="<<(vertices*32+indices*2)/1048576.<<'\n';
        std::cout<<"  planar_interior_vertices="<<planar_interior_vertices
            <<" boundary_locked_triangle_estimate="<<(indices/3-2*planar_interior_vertices)<<'\n';
    }
};
// Match island_demo_scene.cpp::editor_palette_mesh for the actual seed attributes.
terrain_mesh_data palette_mesh(terrain_mesh_data mesh) {
    std::array<std::array<float,4>,24> palette;
    for(int i=0;i<24;++i)palette[i]=editor_climate_color(ClimateType::Temperate,i<8?i-8:i-7);
    for(auto& v:mesh.vertices) {
        float best=1e30f,entry=0;
        for(int i=0;i<23;++i) {
            float numerator=0,denominator=0;
            for(int k=0;k<3;++k){float delta=palette[i+1][k]-palette[i][k];numerator+=(v.color[k]/255.f-palette[i][k])*delta;denominator+=delta*delta;}
            float t=denominator>.000001f?std::max(0.f,std::min(1.f,numerator/denominator)):0;
            float error=0;for(int k=0;k<3;++k){float d=v.color[k]/255.f-(palette[i][k]+t*(palette[i+1][k]-palette[i][k]));error+=d*d;}
            if(error<best){best=error;entry=i+t;}
        }
        v.editor_palette={{(entry+.5f)/24.f,.5f/7.f}};v.color={{255,255,255,255}};
    }
    return mesh;
}
Counts parts(const std::vector<terrain_mesh_data>& meshes) {
    Counts result;for(const auto& mesh:meshes)result.add(mesh);return result;
}
}
int main() {
    try {
        using namespace game;
        std::cout<<std::fixed<<std::setprecision(3);
        terrain_config config;
        std::cout<<"Config world="<<config.world_size<<" island="<<config.island_size
            <<" resolution="<<config.resolution<<" cell="<<config.world_size/config.resolution
            <<" levels="<<config.layers_count<<"+"<<config.underwater_layers_count<<'\n';
        auto data=terrain_generator::generate(config);
        auto surface=terrain_mesh_builder::build(data);
        Counts seed;seed.add(surface);seed.print("seed_raw");
        {
            auto grass=terrain_mesh_builder::build_grass(surface,config);
            Counts counts;for(const auto& mesh:grass)counts.add(mesh,false);
            counts.print("seed_grass");
        }
        auto state=std::make_shared<TerrainEditorState>();state->initialize(data);state->consume_dirty();
        TerrainEditorRenderCache cache(state);
        auto start=Clock::now();cache.initialize(palette_mesh(surface),{});
        auto batches=cache.rebuild({});Counts initial;
        for(const auto& batch:batches)if(batch.kind==EditorRenderBatch::Terrain)initial.add(batch.mesh);
        initial.print("seed_render_groups");
        if(std::getenv("GB_PROFILE_TERRAIN_LOD")){
            TerrainLodSettings settings;settings.set_lod_count(8);settings.set_contour_percent(90);
            Counts coarse;for(const auto& batch:batches)if(batch.kind==EditorRenderBatch::Terrain){
                TerrainContourConstraints constraints;
                auto normalized=batch.mesh;for(auto& v:normalized.vertices)if(v.editor_palette[0]>=0){v.editor_palette={{0,0}};v.position[0]=std::round(v.position[0]*1024.f)/1024.f;v.position[2]=std::round(v.position[2]*1024.f)/1024.f;}auto optimized=optimize_terrain_mesh(normalized,{});auto levels=build_terrain_contour_lods(optimized.parts[0],settings,constraints);
                coarse.add(levels.back().mesh);
            }
            coarse.print("coarsest_contour_90");return 0;
        }
        std::cout<<"seed_cache_cpu_ms="<<std::chrono::duration<double,std::milli>(Clock::now()-start).count()<<'\n';
        batches.clear();
        auto canonical=[&](const char* label) {
            Counts result;size_t flat_chunks=0;size_t flat_triangles=0;
            const int count=(config.resolution+15)/16;
            const int groups=(count+3)/4;
            std::vector<terrain_mesh_data> grouped(groups*groups);
            auto start=Clock::now();
            for(int z=0;z<count;++z)for(int x=0;x<count;++x) {
                auto mesh=build_editor_chunk(state->level,x,z,false).terrain;
                if(mesh.indices.empty())continue;
                result.add(mesh);
                auto& group=grouped[(z/4)*groups+x/4];
                if(group.vertices.size()+mesh.vertices.size()>65535)
                    throw std::runtime_error("Audit render group requires a second 16-bit part");
                const size_t base=group.vertices.size();
                group.vertices.insert(group.vertices.end(),mesh.vertices.begin(),mesh.vertices.end());
                for(auto index:mesh.indices)group.indices.push_back(static_cast<uint16_t>(base+index));
                bool flat=true;const auto& a=mesh.vertices.front();
                for(const auto& v:mesh.vertices)if(v.normal[1]<.99f||v.position[1]!=a.position[1]
                    ||v.outline_id!=a.outline_id||v.editor_palette!=a.editor_palette){flat=false;break;}
                if(flat){++flat_chunks;flat_triangles+=mesh.indices.size()/3;}
            }
            result.print(label);
            std::cout<<"flat_uniform_chunks="<<flat_chunks<<" flat_triangles="<<flat_triangles
                <<" all_chunk_cpu_ms="<<std::chrono::duration<double,std::milli>(Clock::now()-start).count()<<'\n';
            Counts render;for(const auto& group:grouped)render.add(group);
            render.print("  canonical_render_groups");
            Counts optimized;for(const auto& group:grouped)if(!group.indices.empty()){auto compact=optimize_terrain_mesh(group,{});for(const auto& part:compact.parts)optimized.add(part);}optimized.print("  canonical_optimized_render_groups");
        };
        const auto flat=build_editor_chunk(state->level,config.resolution/32,config.resolution/32,false).terrain;
        Counts single;single.add(flat);single.print("central_flat_16x16_chunk");
        canonical("all_canonical_unedited");
        state->current_tool=TerrainEditorTool::Terrain;state->brush_size=32;state->set_target_terrace(7);
        state->begin_stroke();const bool changed=state->paint(12,2);state->end_stroke();
        if(!changed)throw std::runtime_error("Audit edit did not modify the island");
        start=Clock::now();batches=cache.rebuild(state->consume_dirty());
        Counts updates;for(const auto& batch:batches)if(batch.kind==EditorRenderBatch::Terrain)updates.add(batch.mesh);
        updates.print("local_edit_uploaded_terrain");parts(cache.terrain_parts()).print("local_edit_cached_terrain");
        const auto& profile=cache.last_profile();
        std::cout<<"edit_chunks="<<profile.chunks<<" worker_mesh_ms="<<profile.mesh_ms
            <<" batch_ms="<<profile.batch_ms<<" audit_total_ms="<<std::chrono::duration<double,std::milli>(Clock::now()-start).count()<<'\n';
        canonical("all_canonical_after_edit");
        std::cout<<"CPU mesh counts only; not visible triangles, GPU timings or FPS.\n";
        return 0;
    } catch(const std::exception& error) {std::cerr<<error.what()<<'\n';return 1;}
}
