#include "terrain_generator.h"
#include <cmath>
#include <stdexcept>

namespace game
{
    void terrain_config::validate() const
    {
        if (!std::isfinite(world_size) || world_size <= 0.f ||
            resolution < 32 || resolution > 1024 || layers_count < 1 || layers_count > 8 ||
            !std::isfinite(layer_height) || layer_height <= 0.f ||
            !std::isfinite(water_y) || water_y >= 0.f ||
            !std::isfinite(water_size) || water_size < world_size)
            throw std::invalid_argument("Invalid island terrain configuration");
    }

    int terrain_data::layer_at(int x, int z) const
    {
        if (x < 0 || z < 0 || x >= config.resolution || z >= config.resolution) return -1;
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

    terrain_data terrain_generator::generate(const terrain_config& config)
    {
        config.validate();
        terrain_data data;
        data.config = config;
        data.layers.resize(static_cast<size_t>(config.resolution) * config.resolution, -1);
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
            const float coast = config.world_size * (.405f + .047f * std::sin(3.f*angle+p1)
                                                       + .025f * std::sin(5.f*angle+p2)
                                                       + .022f * std::cos(2.f*angle+p3));
            for (int layer = 0; layer < config.layers_count; ++layer)
            {
                const float t = layer / static_cast<float>(config.layers_count);
                const float radius = coast * (1.f - .82f*t)
                    + config.world_size * .012f * t * std::sin(4.f*angle+p2+layer*.65f);
                if (distance <= radius) data.layers[static_cast<size_t>(z)*config.resolution+x] = static_cast<int8_t>(layer);
            }
        }
        return data;
    }
}
