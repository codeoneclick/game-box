#include "building_placement.h"
#include "terrain_generator.h"
#include "terrain_mesh_builder.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

using namespace game;
namespace {
constexpr float pi = 3.14159265358979323846f;
constexpr float passage = 4.f;
void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}
// Nine conservative full-model rectangles: three parallel rows, each with
// three buildings. The same clearance envelopes used by placement must fit,
// and adjacent envelopes have a further four metres of free space.
bool patch_fits(const BuildingSurfaceSupport& support, float x, float z,
                float yaw, float height, float w, float d) {
    const float c = std::cos(yaw), s = std::sin(yaw);
    for (int row = -1; row <= 1; ++row)
    for (int column = -1; column <= 1; ++column) {
        const float u = column * (2*w + passage), v = row * (2*d + passage);
        if (!support.covers(x+c*u+s*v, z-s*u+c*v, yaw, w, d, height))
            return false;
    }
    return true;
}

bool measure(const terrain_config& config, bool print) {
    const auto data = terrain_generator::generate(config);
    const auto mesh = terrain_mesh_builder::build(data);
    BuildingSurfaceSupport support({mesh});
    float w = 0, d = 0;
    for (auto type : {BuildingType::Forester, BuildingType::Stonecutter,
                      BuildingType::Woodcutter}) {
        w = std::max(w, building_definition(type).half_width);
        d = std::max(d, building_definition(type).half_depth);
    }
    w += building_clearance;
    d += building_clearance;
    bool all = true;
    for (int layer = 0; layer < config.layers_count; ++layer) {
        const float height = layer == 0 ? config.shore_height : layer*config.layer_height;
        int sectors = 0;
        for (int sector = 0; sector < 16; ++sector) {
            const float theta = 2*pi*(sector+.37f)/16;
            const float nx = std::cos(theta), nz = std::sin(theta);
            bool fits = false;
            // Candidate spacing is measured in world metres, independent of
            // height-map resolution. A sector counts once, never as capacity
            // for many simultaneously placeable overlapping neighborhoods.
            for (float radius = 0; radius < config.island_size*.55f && !fits; radius += 2.f) {
                const float x = nx*radius, z = nz*radius;
                float sampled_height = 0;
                if (!support.height_at(x,z,sampled_height) ||
                    std::abs(sampled_height-height) > 1e-4f) continue;
                for (float offset : {0.f, -.261799388f, .261799388f}) {
                    // Local X follows the contour tangent; local Z points
                    // outward. Nearby orientations accommodate lobed shores.
                    if (patch_fits(support,x,z,pi*.5f-theta+offset,height,w,d)) {
                        fits = true;
                        break;
                    }
                }
            }
            if (fits) ++sectors;
        }
        if (print) std::cout << "land label " << layer << " (height " << height
                             << "): " << sectors << "/16 sampled sectors fit 3x3\n";
        all = all && sectors > 0;
    }
    if (print) {
        double grass_area = 0;
        // Count the initial grass builder's deterministic sampling stream
        // without allocating millions of cards. Keep random consumption aligned
        // with build_grass: one triangle rounding draw, three candidate draws,
        // and five blade-property draws only for accepted roots.
        uint32_t state = config.seed ^ 0xa531bf79u;
        auto random = [&]() {
            state += 0x9e3779b9u;
            uint32_t value = state;
            value = (value ^ (value >> 16))*0x7feb352du;
            value = (value ^ (value >> 15))*0x846ca68bu;
            value ^= value >> 16;
            return (value & 0xffffffu)/16777216.f;
        };
        size_t accepted_roots = 0;
        for (size_t i = 0; i+2 < mesh.indices.size(); i += 3) {
            const auto& a = mesh.vertices[mesh.indices[i]];
            const auto& b = mesh.vertices[mesh.indices[i+1]];
            const auto& c = mesh.vertices[mesh.indices[i+2]];
            const int label = int(std::round(a.position[1]/config.layer_height));
            const int terrace = config.layers_count-label;
            if (a.normal[1]<.99f || b.normal[1]<.99f || c.normal[1]<.99f ||
                label<1 || terrace<4 || terrace>7) continue;
            const float ux=b.position[0]-a.position[0], uz=b.position[2]-a.position[2];
            const float vx=c.position[0]-a.position[0], vz=c.position[2]-a.position[2];
            const float area=std::abs(ux*vz-uz*vx)*.5f;
            grass_area += area;
            const int candidates=int(std::floor(area*8.3f+random()));
            for (int root=0; root<candidates; ++root) {
                const float r=std::sqrt(random()), s=random();
                const float wb=.97f*r*(1-s)+.01f, wc=.97f*r*s+.01f;
                const float x=a.position[0]+wb*ux+wc*vx;
                const float z=a.position[2]+wb*uz+wc*vz;
                const float patch=.5f+.5f*std::sin(x*.24f+std::sin(z*.13f))*std::cos(z*.21f);
                if(random()>.12f+.88f*patch*patch) continue;
                ++accepted_roots;
                for(int property=0; property<5; ++property) random();
            }
        }
        std::cout << "terrain: " << mesh.vertices.size() << " vertices, " << mesh.indices.size()
                  << " indices; CPU vertex bytes " << sizeof(terrain_vertex) << "\n"
                  << "grass eligible area " << grass_area << " m2; pre-thinning roots ~= "
                  << grass_area*8.3 << "; pre-thinning CPU vertex/index bytes ~= "
                  << grass_area*8.3*(4*sizeof(terrain_vertex)+6*sizeof(uint16_t)) << "\n";
        std::cout << "simulated initial grass roots after patch thinning " << accepted_roots
                  << "; CPU vertex/index payload bytes "
                  << accepted_roots*(4*sizeof(terrain_vertex)+6*sizeof(uint16_t))
                  << "; batches " << (accepted_roots+14999)/15000 << "\n";
        if (config.island_size>=1792.f) {
            // Match the demo's two reference buildings, including their
            // original terrace labels (editor levels are generated labels+1).
            const float scale=config.island_size/256.f;
            for (int house=0; house<2; ++house) {
                const int label=house==0?3:7;
                const auto& definition=building_definition(house==0?BuildingType::Forester:BuildingType::Woodcutter);
                float best=1e30f, chosen_x=0, chosen_z=0;
                for(int dz=-24; dz<=24; dz+=2) for(int dx=-24; dx<=24; dx+=2) {
                    const float x=((house==0?48.f:-8.f)+dx)*scale;
                    const float z=((house==0?40.f:-5.f)+dz)*scale;
                    const int cx=int((x/config.world_size+.5f)*config.resolution);
                    const int cz=int((z/config.world_size+.5f)*config.resolution);
                    const float score=float(dx*dx+dz*dz);
                    if(score>=best || data.layer_at(cx,cz)!=label) continue;
                    if(support.covers(x,z,pi*.75f,definition.half_width+building_clearance,
                                      definition.half_depth+building_clearance,label*config.layer_height)) {
                        best=score; chosen_x=x; chosen_z=z;
                    }
                }
                require(best<1e30f,"Scaled reference-house search must find its original terrace");
                std::cout << "reference house editor level " << label+1 << " x=" << chosen_x
                          << " z=" << chosen_z << '\n';
            }
        }
    }
    return all;
}
}
int main() {
    try {
        const terrain_config current;
        require(std::abs(current.island_size-1792.f*.67f)<.001f,
                "Default terraces must be 33 percent narrower than the expanded island");
        require(std::abs(current.underwater_terrace_width-6.f*.67f)<.0001f,
                "Underwater terrace widths must shrink by the same 33 percent");
        require(current.layers_count==8&&current.layer_height==2.f,
                "Keep all eight terrace heights unchanged");
        terrain_config legacy;
        legacy.world_size=512; legacy.island_size=256; legacy.resolution=512;
        legacy.water_size=768;
        require(!measure(legacy,false), "Legacy narrow terraces unexpectedly satisfy capacity fixture");
        terrain_config expanded;
        expanded.world_size=2048; expanded.island_size=1792;
        expanded.resolution=1024; expanded.water_size=2048;
        expanded.underwater_terrace_width=6.f;
        require(measure(expanded,true), "Expanded island fixture must fit three-row neighborhoods");
        require(measure(current,false), "Every dry terrace must fit a three-row building neighborhood");
        std::cout << "Island capacity CPU checks passed (sampled neighborhoods, not every point).\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
