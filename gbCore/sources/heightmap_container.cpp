//
//  heightmap_container.cpp
//  gbCore
//
//  Created by serhii serhiiv on 8/24/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "heightmap_container.h"

namespace gb
{
    heightmap_container::heightmap_container(const glm::ivec2& heightmap_size) :
    m_heightmap_size(heightmap_size)
    {
        m_uncompressed_vertices = new uncomressed_vertex[m_heightmap_size.x * m_heightmap_size.y];
        m_compressed_vertices = new compressed_vertex[m_heightmap_size.x * m_heightmap_size.y];
        m_faces = new face[(m_heightmap_size.x - 1) * (m_heightmap_size.y - 1) * 2];
    }
}
