#include "terrain_generator.h"
#include "terrain_mesh_builder.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

int main()
{
    game::terrain_config config;
    const auto data = game::terrain_generator::generate(config);
    assert(data.layers == game::terrain_generator::generate(config).layers);
    auto other = config;
    ++other.seed;
    assert(data.layers != game::terrain_generator::generate(other).layers);
    bool present[8] = {};
    for (const auto layer : data.layers)
    {
        assert(layer >= -1 && layer < 8);
        if (layer >= 0) present[layer] = true;
    }
    for (bool value : present) assert(value);
    const auto mesh = game::terrain_mesh_builder::build(data);
    assert(!mesh.vertices.empty() && mesh.vertices.size() <= 65535);
    assert(mesh.indices.size() % 3 == 0);
    bool tops[8] = {};
    bool walls = false;
    for (const auto& vertex : mesh.vertices)
    {
        for (float value : vertex.position) assert(std::isfinite(value));
        if (vertex.normal[1] > .99f)
        {
            const int layer = static_cast<int>(std::round(vertex.position[1] / config.layer_height));
            assert(layer >= 0 && layer < 8);
            assert(std::abs(vertex.position[1] - layer * config.layer_height) < .0001f);
            tops[layer] = true;
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
    const auto reject_data = [](const game::terrain_data& invalid) {
        bool failed = false;
        try { game::terrain_mesh_builder::build(invalid); }
        catch (const std::invalid_argument&) { failed = true; }
        assert(failed);
    };
    auto edited = data;
    edited.layers.pop_back(); reject_data(edited);
    edited = data; edited.layers[0] = 8; reject_data(edited);
    edited = data; edited.layers[253 * 256 + 254] = 0; reject_data(edited);
    edited = data; edited.layers[128 * 256 + 136] = -1; reject_data(edited);
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
