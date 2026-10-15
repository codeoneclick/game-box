#include "terrain_generator.h"
#include "terrain_mesh_builder.h"
#include "grass_interaction.h"
#include "orbit_zoom.h"
#include "outline_ids.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

int main()
{
    game::orbit_zoom zoom;
    zoom.scroll(1.f);
    assert(zoom.radius==245.f && zoom.target<zoom.radius);
    assert(zoom.advance(1.f/60.f) && zoom.radius>zoom.target && zoom.radius<245.f);
    game::orbit_zoom reference;reference.scroll(3.f);
    for(int i=0;i<120;++i)reference.advance(1.f/120.f);
    for(int rate:{30,60,144}){
        game::orbit_zoom candidate;candidate.scroll(3.f);
        for(int i=0;i<rate;++i)candidate.advance(1.f/rate);
        assert(std::abs(candidate.radius-reference.radius)<.005f);
    }
    zoom.reset(245.f,15.f,700.f);zoom.scroll(1000.f);
    assert(zoom.target==15.f);
    for(int i=0;i<120;++i){zoom.advance(1.f/60.f);assert(zoom.radius>=15.f&&zoom.radius<=700.f);}
    assert(std::abs(zoom.radius-15.f)<.001f);
    zoom.scroll(-1000.f);assert(zoom.target==700.f);
    zoom.advance(5.f);assert(std::abs(zoom.radius-700.f)<.001f);
    const float unchanged_zoom=zoom.radius;
    zoom.scroll(std::numeric_limits<float>::infinity());
    zoom.advance(std::numeric_limits<float>::quiet_NaN());
    zoom.advance(-1.f);assert(zoom.radius==unchanged_zoom);
    zoom.reset(3.5f,2.5f,700.f);assert(zoom.radius==zoom.target&&zoom.radius==3.5f);
    std::cout<<"Spring zoom checks passed: smooth start, frame-rate independence, limits and invalid input\n";
    game::grass_interaction interaction;
    std::vector<game::grass_impulse> people(200,{0,0,2,1,0,0});
    for(int i=0;i<200;++i){interaction.update(.05f,people);assert(interaction.trail().size()<=2600);}
    for(int i=0;i<40;++i)interaction.update(.05f,{});
    assert(interaction.trail().empty());
    assert(game::grass_interaction::strength(0)==1.f);
    assert(game::grass_interaction::strength(.3f)>game::grass_interaction::strength(1.f));
    assert(game::grass_interaction::strength(1.5f)==0.f);
    game::terrain_config config;
    const auto data = game::terrain_generator::generate(config);
    assert(data.layers == game::terrain_generator::generate(config).layers);
    auto other = config;
    ++other.seed;
    assert(data.layers != game::terrain_generator::generate(other).layers);
    assert(config.underwater_layers_count == 8);
    assert(data.layer_at(-1, 0) == game::terrain_data::empty_layer);
    bool present[16] = {};
    for (const auto layer : data.layers)
    {
        assert(layer == game::terrain_data::empty_layer || (layer >= -8 && layer < 8));
        if (layer != game::terrain_data::empty_layer) present[layer+8] = true;
    }
    for (bool value : present) assert(value);
    const auto mesh = game::terrain_mesh_builder::build(data);
    for(size_t i=0;i+2<mesh.indices.size();i+=3){
        const auto& a=mesh.vertices[mesh.indices[i]];
        const auto& b=mesh.vertices[mesh.indices[i+1]];
        const auto& c=mesh.vertices[mesh.indices[i+2]];
        assert(a.outline_id>=2&&a.outline_id<=40&&a.outline_id==b.outline_id&&a.outline_id==c.outline_id);
        if(a.normal[1]>.99f){const int logical=a.position[1]==config.shore_height?1:int(std::round(a.position[1]/config.layer_height))+(a.position[1]>0?1:0);assert(a.outline_id==game::outline_terrace_id(logical));}
    }
    const auto trees=game::terrain_mesh_builder::build_trees(mesh,config);
    const auto repeated_trees=game::terrain_mesh_builder::build_trees(mesh,config);
    assert(trees.size()==repeated_trees.size());
    int tree_counts[8]={},pine_counts[8]={};
    size_t tree_triangles=0;
    for(size_t i=0;i<trees.size();++i){
        const auto& tree=trees[i];
        assert(tree.terrace>=3&&tree.terrace<=7);
        ++tree_counts[tree.terrace];pine_counts[tree.terrace]+=tree.conifer?1:0;
        assert(tree.position==repeated_trees[i].position);
        assert(tree.mesh.indices==repeated_trees[i].mesh.indices);
        assert(std::abs(tree.position[1]-(config.layers_count-tree.terrace)*config.layer_height)<.01f);
        assert(tree.mesh.vertices.size()<=65535 && tree.mesh.indices.size()/3<2000);
        bool fixed_root=false,bending_crown=false;
        for(size_t v=0;v<tree.mesh.vertices.size();++v){
            const auto& vertex=tree.mesh.vertices[v];
            assert(vertex.outline_id==game::outline_tree_id(uint32_t(i+1)));
            assert(vertex.outline_id==repeated_trees[i].mesh.vertices[v].outline_id);
            assert(vertex.position==repeated_trees[i].mesh.vertices[v].position);
            for(float p:vertex.position)assert(std::isfinite(p));
            assert(vertex.texcoord[0]==tree.position[0]&&vertex.texcoord[1]==tree.position[2]);
            assert(vertex.tree_bend[0]>=0.f&&vertex.tree_bend[0]<=1.f);
            fixed_root|=vertex.tree_bend[0]==0.f;
            bending_crown|=vertex.tree_bend[0]>.85f;
        }
        assert(fixed_root&&bending_crown);
        for(auto index:tree.mesh.indices)assert(index<tree.mesh.vertices.size());
        for(size_t j=0;j<i;++j)if(trees[j].terrace==tree.terrace){
            const float dx=trees[j].position[0]-tree.position[0],dz=trees[j].position[2]-tree.position[2];
            assert(dx*dx+dz*dz>=25.f);
        }
        tree_triangles+=tree.mesh.indices.size()/3;
    }
    for(int terrace=3;terrace<=7;++terrace)assert(tree_counts[terrace]>=10&&tree_counts[terrace]<=20);
    assert(pine_counts[3]*2>tree_counts[3]&&pine_counts[4]*2>tree_counts[4]);
    assert(pine_counts[6]*2<tree_counts[6]&&pine_counts[7]*2<tree_counts[7]);
    std::cout<<"Tree checks passed: "<<trees.size()<<" trees, "<<tree_triangles<<" triangles\n";
    assert(!mesh.vertices.empty() && mesh.vertices.size() <= 65535);
    assert(mesh.indices.size() % 3 == 0);
    const auto grass = game::terrain_mesh_builder::build_grass(mesh, config);
    const auto repeated_grass = game::terrain_mesh_builder::build_grass(mesh, config);
    assert(!grass.empty() && grass.size() == repeated_grass.size());
    bool grass_levels[8] = {};
    size_t grass_blades = 0;
    for (size_t batch = 0; batch < grass.size(); ++batch)
    {
        const auto& patch = grass[batch];
        assert(!patch.vertices.empty() && patch.vertices.size() <= 65535);
        assert(patch.vertices.size() % 4 == 0 && patch.indices.size() == patch.vertices.size()/4*6);
        assert(patch.indices == repeated_grass[batch].indices);
        for (size_t i = 0; i < patch.vertices.size(); ++i)
        {
            const auto& vertex = patch.vertices[i];
            assert(vertex.outline_id==0);
            assert(vertex.position == repeated_grass[batch].vertices[i].position);
            for (float value : vertex.position) assert(std::isfinite(value));
        }
        for (auto index : patch.indices) assert(index < patch.vertices.size());
        for (size_t i = 0; i < patch.vertices.size(); i += 4)
        {
            const float y = patch.vertices[i].position[1];
            const int layer = static_cast<int>(std::round(y/config.layer_height));
            assert(layer >= 1 && layer <= 4); // Terraces 4..7, numbered from the summit.
            assert(std::abs(y-layer*config.layer_height) < .01f);
            grass_levels[layer] = true;
            const float height = patch.vertices[i+3].position[1]-y;
            assert(height >= .25f && height <= .6f);
            assert(patch.vertices[i+3].color[1] > patch.vertices[i].color[1]);
            assert(patch.vertices[i].grass_bend[0] == 0.f && patch.vertices[i+1].grass_bend[0] == 0.f);
            for(size_t vertex=0;vertex<4;++vertex)
                assert(patch.vertices[i+vertex].color[3]==(vertex%2?255:0));
            assert(patch.vertices[i+3].grass_bend[0] == 1.f);
            assert(std::abs(patch.vertices[i].grass_bend[1]-height) < .00001f);
            for(size_t vertex=1;vertex<4;++vertex){
                assert(patch.vertices[i+vertex].grass_bend[1]==patch.vertices[i].grass_bend[1]);
                assert(patch.vertices[i+vertex].texcoord==patch.vertices[i].texcoord);
            }
            ++grass_blades;
        }
    }
    for (int layer = 1; layer <= 4; ++layer) assert(grass_levels[layer]);
    std::cout << "Grass geometry checks passed: " << grass_blades << " blades, " << grass.size() << " batches\n";
    bool tops[16] = {};
    bool walls = false;
    for (const auto& vertex : mesh.vertices)
    {
        for (float value : vertex.position) assert(std::isfinite(value));
        if (vertex.normal[1] > .99f)
        {
            const int layer = static_cast<int>(std::round(vertex.position[1] / config.layer_height));
            assert(layer >= -8 && layer < 8);
            const float expected_height=layer==0 ? config.shore_height : layer*config.layer_height;
            assert(std::abs(vertex.position[1] - expected_height) < .0001f);
            tops[layer+8] = true;
        }
        else { assert(std::abs(vertex.normal[1]) < .0001f); walls = true; }
    }
    for (bool value : tops) assert(value);
    assert(walls);
    for (size_t i = 0; i < mesh.indices.size(); i += 3)
    {
        for (size_t j = 0; j < 3; ++j) assert(mesh.indices[i+j] < mesh.vertices.size());
        const auto& a = mesh.vertices[mesh.indices[i]];
        const auto& b = mesh.vertices[mesh.indices[i+1]];
        const auto& c = mesh.vertices[mesh.indices[i+2]];
        const float ux=b.position[0]-a.position[0], uy=b.position[1]-a.position[1], uz=b.position[2]-a.position[2];
        const float vx=c.position[0]-a.position[0], vy=c.position[1]-a.position[1], vz=c.position[2]-a.position[2];
        const float dot=(uy*vz-uz*vy)*a.normal[0]+(uz*vx-ux*vz)*a.normal[1]+(ux*vy-uy*vx)*a.normal[2];
        assert(dot > 0.f);
    }
    other.resolution = 64;
    game::terrain_mesh_builder::build(game::terrain_generator::generate(other));
    other.resolution = 0;
    bool rejected = false;
    try { game::terrain_generator::generate(other); }
    catch (const std::invalid_argument&) { rejected = true; }
    assert(rejected);
    for (uint32_t seed : {0u, 1u, 42u, 0xffffffffu})
    {
        other = config;
        other.seed = seed;
        other.resolution = 32;
        game::terrain_mesh_builder::build(game::terrain_generator::generate(other));
    }
    other = config;
    other.resolution = 1024;
    assert(game::terrain_mesh_builder::build(game::terrain_generator::generate(other)).vertices.size() <= 65535);
    other.layers_count = 1;
    other.underwater_layers_count = 0;
    game::terrain_mesh_builder::build(game::terrain_generator::generate(other));
    const auto reject_config = [](game::terrain_config invalid) {
        bool failed = false;
        try { game::terrain_generator::generate(invalid); }
        catch (const std::invalid_argument&) { failed = true; }
        assert(failed);
    };
    other = config; other.world_size = std::numeric_limits<float>::quiet_NaN(); reject_config(other);
    other = config; other.layers_count = 9; reject_config(other);
    other = config; other.layer_height = 0.f; reject_config(other);
    other = config; other.water_y = 0.f; reject_config(other);
    other = config; other.underwater_layers_count = 9; reject_config(other);
    const auto reject_data = [](const game::terrain_data& invalid) {
        bool failed = false;
        try { game::terrain_mesh_builder::build(invalid); }
        catch (const std::invalid_argument&) { failed = true; }
        assert(failed);
    };
    auto edited = data;
    edited.layers.pop_back(); reject_data(edited);
    edited = data; edited.layers[0] = 8; reject_data(edited);
    edited = data; edited.layers[0] = 0; reject_data(edited);
    edited = data; edited.layers[(config.resolution/2)*config.resolution+config.resolution/2] = game::terrain_data::empty_layer; reject_data(edited);
    edited = data;
    // Rotation of logical cells must rotate the mesh without regenerating it.
    for (int z=0; z<config.resolution; ++z)
    for (int x=0; x<config.resolution; ++x)
        edited.layers[static_cast<size_t>(z)*config.resolution+x] = data.layer_at(z,config.resolution-1-x);
    const auto edited_mesh = game::terrain_mesh_builder::build(edited);
    assert(edited_mesh.vertices[0].position != mesh.vertices[0].position);
    assert(game::terrain_mesh_builder::build_water(config).indices.size() == 6);
    std::cout << "Island CPU smoke checks passed: " << mesh.vertices.size()
              << " vertices, " << mesh.indices.size()/3 << " triangles\n";
}
