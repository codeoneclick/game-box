#pragma once
#include "terrain_data.h"

namespace game
{
    class terrain_generator
    {
    public:
        static float coast_radius(const terrain_config&,float angle);
        static terrain_data generate(const terrain_config& config);
    };
}
