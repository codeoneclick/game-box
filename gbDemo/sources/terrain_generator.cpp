#include "terrain_generator.h"
#include <cmath>
#include <algorithm>
#include <stdexcept>

namespace game
{
    void terrain_config::validate() const
    {
        if (!std::isfinite(world_size) || world_size <= 0.f ||
            resolution < 32 || resolution > 1024 || layers_count < 1 || layers_count > 8 ||
            underwater_layers_count < 0 || underwater_layers_count > 8 ||
            !std::isfinite(island_size) || island_size <= 0.f ||
            !std::isfinite(underwater_terrace_width) || underwater_terrace_width <= 0.f ||
            world_size < island_size + 2.f*underwater_layers_count*underwater_terrace_width ||
            !std::isfinite(layer_height) || layer_height <= 0.f ||
            !std::isfinite(shore_height) || shore_height < 0.f || shore_height >= layer_height ||
            !std::isfinite(water_y) || water_y >= 0.f ||
            (underwater_layers_count > 0 && water_y <= -layer_height) ||
            !std::isfinite(water_size) || water_size < world_size)
            throw std::invalid_argument("Invalid island terrain configuration");
    }

    int terrain_data::layer_at(int x, int z) const
    {
        if (x < 0 || z < 0 || x >= config.resolution || z >= config.resolution) return empty_layer;
        return layers[static_cast<size_t>(z) * config.resolution + x];
    }

    namespace
    {
        // Explicit integer hash avoids implementation-dependent random distributions.
        float phase(uint32_t seed, uint32_t salt)
        {
            uint32_t value = seed ^ (salt * 0x9e3779b9u);
            value ^= value >> 16; value *= 0x7feb352du;
            value ^= value >> 15; value *= 0x846ca68bu;
            value ^= value >> 16;
            return (value & 0xffffu) * (6.283185307f / 65536.f);
        }
    }

    float terrain_generator::coast_radius(const terrain_config& config,float angle)
    {
        const float p1=phase(config.seed,1),p2=phase(config.seed,2),p3=phase(config.seed,3);
        return config.island_size*(.405f+.047f*std::sin(3.f*angle+p1)+.025f*std::sin(5.f*angle+p2)+.022f*std::cos(2.f*angle+p3));
    }
    terrain_data terrain_generator::generate(const terrain_config& config)
    {
        config.validate();
        terrain_data data;
        data.config = config;
        data.layers.resize(static_cast<size_t>(config.resolution) * config.resolution, static_cast<int8_t>(terrain_data::empty_layer));
        const float cell = config.world_size / config.resolution;
        const float p1 = phase(config.seed, 1), p2 = phase(config.seed, 2), p3 = phase(config.seed, 3);
        for (int z = 0; z < config.resolution; ++z)
        for (int x = 0; x < config.resolution; ++x)
        {
            const float px = (x + .5f) * cell - config.world_size * .5f;
            const float pz = (z + .5f) * cell - config.world_size * .5f;
            const float angle = std::atan2(pz, px);
            const float distance = std::sqrt(px * px + pz * pz);
            // Broad bays/lobes; the decreasing base radius ensures nested layers.
            const float coast = config.island_size * (.405f + .047f * std::sin(3.f*angle+p1)
                                                       + .025f * std::sin(5.f*angle+p2)
                                                       + .022f * std::cos(2.f*angle+p3));
            for (int layer = -config.underwater_layers_count; layer < config.layers_count; ++layer)
            {
                const float t = std::max(layer,0) / static_cast<float>(config.layers_count);
                const float radius = layer < 0 ? coast + (-layer)*config.underwater_terrace_width
                    * (1.f + .1f*std::sin(4.f*angle+p3)) : coast * (1.f - .82f*t)
                    + config.island_size * .012f * t * std::sin(4.f*angle+p2+layer*.65f);
                // Shift upper plateaus gently: varying widths, while keeping each
                // terrace nested and its logical walking/building height flat.
                const float shift_x=config.island_size*.005f*t*std::sin(p1+layer*.55f);
                const float shift_z=config.island_size*.005f*t*std::cos(p2+layer*.55f);
                const float terrace_distance=layer<0?distance:std::sqrt((px-shift_x)*(px-shift_x)+(pz-shift_z)*(pz-shift_z));
                if (terrace_distance <= radius) data.layers[static_cast<size_t>(z)*config.resolution+x] = static_cast<int8_t>(layer);
            }
        }
        return data;
    }
}
