#include "terrain_mesh_builder.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace game
{
    namespace
    {
        using point = std::array<float, 3>;
        using color = std::array<uint8_t, 4>;

        // Append independent vertices so tops never share normals with walls.
        void triangle(terrain_mesh_data& mesh, point a, point b, point c, point normal, color tint)
        {
            if (mesh.vertices.size() + 3 > 65535)
                throw std::invalid_argument("Island mesh exceeds 16-bit vertex capacity");
            for (const auto& p : {a, b, c})
            {
                mesh.indices.push_back(static_cast<uint16_t>(mesh.vertices.size()));
                mesh.vertices.push_back({p, normal, tint});
            }
        }

        float occupancy(const terrain_data& data, float x, float z, int layer)
        {
            const float cell = data.config.world_size / data.config.resolution;
            const float gx = (x + data.config.world_size*.5f) / cell - .5f;
            const float gz = (z + data.config.world_size*.5f) / cell - .5f;
            const int ix = static_cast<int>(std::floor(gx)), iz = static_cast<int>(std::floor(gz));
            const float tx = gx-ix, tz = gz-iz;
            const float a = data.layer_at(ix, iz) >= layer ? 1.f : 0.f;
            const float b = data.layer_at(ix+1, iz) >= layer ? 1.f : 0.f;
            const float c = data.layer_at(ix, iz+1) >= layer ? 1.f : 0.f;
            const float d = data.layer_at(ix+1, iz+1) >= layer ? 1.f : 0.f;
            return (a*(1.f-tx)+b*tx)*(1.f-tz)+(c*(1.f-tx)+d*tx)*tz;
        }

        void validate_layer_mask(const terrain_data& data, int layer)
        {
            const int size = data.config.resolution;
            // Flood both land and ocean. This checks every cell, including cells
            // between radial rays, and prevents silently dropping islands/holes.
            for (bool land : {true, false})
            {
                std::vector<uint8_t> visited(data.layers.size(), 0);
                std::vector<size_t> queue;
                queue.reserve(data.layers.size());
                const auto enqueue = [&](int x, int z) {
                    if (x < 0 || z < 0 || x >= size || z >= size) return;
                    const size_t index = static_cast<size_t>(z) * size + x;
                    if (!visited[index] && ((data.layers[index] >= layer) == land))
                    {
                        visited[index] = 1;
                        queue.push_back(index);
                    }
                };
                if (land) enqueue(size / 2, size / 2);
                else for (int i = 0; i < size; ++i)
                {
                    enqueue(i, 0); enqueue(i, size - 1);
                    enqueue(0, i); enqueue(size - 1, i);
                }
                for (size_t head = 0; head < queue.size(); ++head)
                {
                    const int x = static_cast<int>(queue[head] % size);
                    const int z = static_cast<int>(queue[head] / size);
                    enqueue(x - 1, z); enqueue(x + 1, z);
                    enqueue(x, z - 1); enqueue(x, z + 1);
                }
                for (size_t i = 0; i < data.layers.size(); ++i)
                    if (((data.layers[i] >= layer) == land) && !visited[i])
                        throw std::invalid_argument("Terrain layer has disconnected land or a hole");
            }
        }
    }

    terrain_mesh_data terrain_mesh_builder::build(const terrain_data& data)
    {
        const auto& config = data.config;
        config.validate();
        if (data.layers.size() != static_cast<size_t>(config.resolution)*config.resolution)
            throw std::invalid_argument("Terrain cell count does not match resolution");
        for (int layer : data.layers)
            if (layer != terrain_data::empty_layer && (layer < -config.underwater_layers_count || layer >= config.layers_count))
                throw std::invalid_argument("Terrain cell layer out of range");
        const int count = config.layers_count + config.underwater_layers_count;
        for (int layer = -config.underwater_layers_count; layer < config.layers_count; ++layer)
            validate_layer_mask(data, layer);
        const int segments = std::min(config.resolution, 320);
        const float cell = config.world_size/config.resolution;
        std::vector<std::vector<float>> radii(count, std::vector<float>(segments));
        for (int layer = 0; layer < count; ++layer)
        for (int i = 0; i < segments; ++i)
        {
            const float angle = i*6.283185307f/segments;
            const float dx = std::cos(angle), dz = std::sin(angle);
            const int label = layer - config.underwater_layers_count;
            if (occupancy(data, 0.f, 0.f, label) < .5f)
                throw std::invalid_argument("Terrain layer must contain the origin");
            float boundary = 0.f;
            bool outside = false;
            // Scan, rather than binary-search: reject a second occupied interval.
            for (float r = cell*.25f; r <= config.world_size*.72f; r += cell*.25f)
            {
                const bool inside = occupancy(data, dx*r, dz*r, label) >= .5f;
                if (outside && inside)
                    throw std::invalid_argument("Terrain layer is not star-shaped");
                if (!inside && !outside)
                {
                    float lo = r-cell*.25f, hi = r;
                    for (int step = 0; step < 12; ++step)
                    {
                        const float mid = (lo+hi)*.5f;
                        if (occupancy(data, dx*mid, dz*mid, label) >= .5f) lo=mid;
                        else hi=mid;
                    }
                    boundary=(lo+hi)*.5f;
                    outside=true;
                }
            }
            if (!outside || boundary < cell)
                throw std::invalid_argument("Terrain contour is missing or too small");
            radii[layer][i] = boundary;
        }
        // Positive convolution preserves nesting while removing grid stair steps.
        for (auto& ring : radii)
        for (int pass=0; pass<4; ++pass)
        {
            const auto previous = ring;
            for (int i=0; i<segments; ++i)
                ring[i] = (previous[(i+segments-2)%segments] + 4.f*previous[(i+segments-1)%segments]
                         + 6.f*previous[i] + 4.f*previous[(i+1)%segments] + previous[(i+2)%segments])/16.f;
        }
        terrain_mesh_data mesh;
        // Four triangles per contour segment/layer, except the innermost top fan has one.
        const size_t vertex_count = static_cast<size_t>(4*count-1)*segments*3;
        mesh.vertices.reserve(std::min<size_t>(vertex_count,65535));
        mesh.indices.reserve(std::min<size_t>(vertex_count,65535));
        const point up = {{0.f, 1.f, 0.f}};
        const auto at = [&](int layer, int i, float height) -> point {
            const float angle=i*6.283185307f/segments;
            return {{radii[layer][i]*std::cos(angle), height, radii[layer][i]*std::sin(angle)}};
        };
        for (int layer=0; layer<count; ++layer)
        {
            const int label = layer - config.underwater_layers_count;
            const float y=label==0 ? config.shore_height : label*config.layer_height;
            const int land_label=std::max(label,0);
            const color grass = {{static_cast<uint8_t>(85+land_label*4), static_cast<uint8_t>(145+land_label*5),
                                  static_cast<uint8_t>(69+land_label*3), 255}};
            const color debug = {{static_cast<uint8_t>(35+layer*13), static_cast<uint8_t>(210-layer*9), static_cast<uint8_t>(60+(layer%4)*45), 255}};
            const color soil = {{static_cast<uint8_t>(120+layer*3), static_cast<uint8_t>(85+layer*3),
                                 static_cast<uint8_t>(54+layer*2), 255}};
            for (int i=0; i<segments; ++i)
            {
                const int j=(i+1)%segments;
                const auto a=at(layer,i,y), b=at(layer,j,y);
                const color tint = config.debug_layer_colors ? debug : (label < 0 ? color{{177,163,118,255}} : grass);
                if (layer+1 < count)
                {
                    if (radii[layer][i] <= radii[layer+1][i] || radii[layer][j] <= radii[layer+1][j])
                        throw std::invalid_argument("Terrain terraces must have positive width");
                    const auto c=at(layer+1,i,y), d=at(layer+1,j,y);
                    triangle(mesh,a,c,b,up,tint);
                    triangle(mesh,b,c,d,up,tint);
                }
                else triangle(mesh,a,{{0.f,y,0.f}},b,up,tint);
                const float bottom=layer==0 ? std::min(config.water_y-config.layer_height, y-config.layer_height)
                    : (label==0 ? -config.layer_height : (label==1 ? config.shore_height : y-config.layer_height));
                const auto c=at(layer,i,bottom), d=at(layer,j,bottom);
                const float nx=b[2]-a[2], nz=a[0]-b[0];
                const float length=std::sqrt(nx*nx+nz*nz);
                const point normal={{nx/length,0.f,nz/length}};
                triangle(mesh,a,b,c,normal,soil);
                triangle(mesh,b,d,c,normal,soil);
            }
        }
        return mesh;
    }

    terrain_mesh_data terrain_mesh_builder::build_water(const terrain_config& config)
    {
        config.validate();
        terrain_mesh_data mesh;
        const float s=config.water_size*.5f, y=config.water_y;
        const point up={{0.f,1.f,0.f}};
        const color blue={{48,153,174,255}};
        triangle(mesh,{{-s,y,-s}},{{-s,y,s}},{{s,y,-s}},up,blue);
        triangle(mesh,{{s,y,-s}},{{-s,y,s}},{{s,y,s}},up,blue);
        return mesh;
    }

    terrain_mesh_data terrain_mesh_builder::build_seabed(const terrain_config& config)
    {
        auto mesh = build_water(config);
        for (auto& vertex : mesh.vertices)
        {
            vertex.position[1] = -(config.underwater_layers_count+1)*config.layer_height;
            vertex.color = {{137,135,100,255}};
        }
        return mesh;
    }
}
