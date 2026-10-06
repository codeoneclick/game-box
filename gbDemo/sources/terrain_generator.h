#pragma once
#include "terrain_data.h"

namespace game
{
    class terrain_generator
    {
    public:
        static terrain_data generate(const terrain_config& config);
    };
}
