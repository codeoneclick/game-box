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
#include "stroke.h"
#include "resource_accessor.h"
#include "mesh.h"
#include "material.h"
#include "renderable_game_object.h"
#include "mesh_constructor.h"
#include "texture_configuration.h"
#include "sprite_configuration.h"
#include "configuration_accessor.h"
#include "ces_geometry_freeform_component.h"
#include "ces_convex_hull_component.h"
#include "ces_shadow_component.h"

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
                
                ces_convex_hull_component_shared_ptr convex_hull_component = std::make_shared<ces_convex_hull_component>();
                
                glm::vec4 grid_bound = grid->get_bound();
                
                vbo::vertex_attribute vertices[4];
                vertices[0].m_position = glm::vec2(grid_bound.x, grid_bound.y);
                vertices[1].m_position = glm::vec2(grid_bound.z, grid_bound.y);
                vertices[2].m_position = glm::vec2(grid_bound.z, grid_bound.w);
                vertices[3].m_position = glm::vec2(grid_bound.x, grid_bound.w);
                
                convex_hull_component->create_convex_hull(vertices, 4);
                grid->add_component(convex_hull_component);
                
                ces_shadow_component_shared_ptr shadow_component = std::make_shared<ces_shadow_component>();
                grid->add_component(shadow_component);
                
                m_fabricator->add_materials(grid, grid_configuration->get_materials_configurations());
                
                m_game_objects_container.insert(grid);
            }
            return grid;
        }
        
        stroke_shared_ptr ed_fabricator::create_stroke(const std::string& filename)
        {
            std::shared_ptr<sprite_configuration> stroke_configuration =
            std::static_pointer_cast<gb::sprite_configuration>(m_fabricator->get_configuration_accessor()->get_sprite_configuration(filename));
            assert(stroke_configuration);
            stroke_shared_ptr stroke = nullptr;
            if(stroke_configuration)
            {
                stroke = std::make_shared<gb::ed::stroke>();
                unsafe_get_geometry_freeform_component(stroke)->set_mesh(gb::ed::mesh_constructor::create_stroke());
                
                m_fabricator->add_materials(stroke, stroke_configuration->get_materials_configurations());
                
                m_game_objects_container.insert(stroke);
            }
            return stroke;
        }
    };
};

