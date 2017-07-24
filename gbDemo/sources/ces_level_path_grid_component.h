//
//  ces_level_path_grid_component.h
//  gbDemo
//
//  Created by serhii serhiiv on 7/24/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_level_path_grid_component : public gb::ces_base_component
    {
    private:
        
    protected:
        
        path_map_shared_ptr m_path_grid;
        glm::ivec2 m_level_size;
        glm::ivec2 m_level_cell_size;
        glm::ivec2 m_level_cells_count;
        
    public:
        
        CTTI_CLASS_GUID(ces_level_path_grid_component, gb::ces_base_component::g_guids_container)
        ces_level_path_grid_component(const glm::ivec2& level_size, const glm::ivec2& level_cell_size);
        ~ces_level_path_grid_component();
        
        path_map_shared_ptr get_path_grid() const;
        glm::ivec2 get_level_size() const;
    };
};

