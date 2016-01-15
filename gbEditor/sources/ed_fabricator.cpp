//
//  ed_fabricator.cpp
//  gbEditor
//
//  Created by sergey.sergeev on 1/15/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ed_fabricator.h"
#include "scene_fabricator.h"
#include "ed_mesh_constructor.h"
#include "grid.h"
#include "resource_accessor.h"
#include "mesh.h"
#include "material.h"
#include "renderable_game_object.h"
#include "mesh_constructor.h"
#include "texture_configuration.h"
#include "sprite_configuration.h"
#include "configuration_accessor.h"
#include "ces_geometry_freeform_component.h"

namespace gb
{
    namespace ed
    {
            
        ed_fabricator::ed_fabricator(const scene_fabricator_shared_ptr& fabricator) :
        m_fabricator(fabricator)
        {
            
        }
        ed_fabricator::~ed_fabricator()
        {
        
        }
            
        grid_shared_ptr ed_fabricator::create_grid(const std::string& filename, i32 num_rows, i32 num_columns, f32 rows_gap, f32 columns_gap)
        {
            std::shared_ptr<sprite_configuration> grid_configuration =
            std::static_pointer_cast<gb::sprite_configuration>(m_fabricator->get_configuration_accessor()->get_sprite_configuration(filename));
            assert(grid_configuration);
            grid_shared_ptr grid = nullptr;
            if(grid_configuration)
            {
                grid = std::make_shared<gb::ed::grid>();
                unsafe_get_geometry_freeform_component(grid)->set_mesh(gb::ed::mesh_constructor::create_grid(num_rows, num_columns, rows_gap, columns_gap));
                m_fabricator->add_materials(grid, grid_configuration->get_materials_configurations());
                
                m_game_objects_container.insert(grid);
            }
            return grid;
        }
    };
};

