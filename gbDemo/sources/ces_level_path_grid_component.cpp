//
//  ces_level_path_grid_component.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 7/24/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_level_path_grid_component.h"
#include "path_map.h"

namespace game
{
    ces_level_path_grid_component::ces_level_path_grid_component(const glm::ivec2& level_size, const glm::ivec2& level_cell_size) :
    m_level_size(level_size),
    m_level_cell_size(level_cell_size)
    {
        assert(level_size.x % level_cell_size.x == 0 &&
               level_size.y % level_cell_size.y == 0);
        m_level_cells_count.x = level_size.x / level_cell_size.x;
        m_level_cells_count.y = level_size.y / level_cell_size.y;
        m_path_grid = std::make_shared<game::path_map>(m_level_cells_count, level_cell_size);
        for(i32 i = 0; i < m_level_cells_count.x; ++i)
        {
            for(i32 j = 0; j < m_level_cells_count.y; ++j)
            {
                m_path_grid->set_path_passable(i, j, true);
            }
        }
    }
    
    ces_level_path_grid_component::~ces_level_path_grid_component()
    {
        
    }
    
    path_map_shared_ptr ces_level_path_grid_component::get_path_grid() const
    {
        return m_path_grid;
    }
}
