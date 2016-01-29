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
#include "terrain.h"
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
        
        terrain_shared_ptr ed_fabricator::create_terrain(const std::string& filename, const glm::vec2& size,
                                                         const std::string& mask_texture_filename,
                                                         const std::vector<std::string>& diffuse_textures_filenames,
                                                         const std::vector<std::string>& normalmap_textures_filenames)
        {
            std::shared_ptr<sprite_configuration> terrain_configuration =
            std::static_pointer_cast<gb::sprite_configuration>(m_fabricator->get_configuration_accessor()->get_sprite_configuration(filename));
            assert(terrain_configuration);
            terrain_shared_ptr terrain = nullptr;
            if(terrain_configuration)
            {
                terrain = std::make_shared<gb::ed::terrain>();
                m_fabricator->add_materials(terrain, terrain_configuration->get_materials_configurations());
                
                terrain->set_size(size);
                
                assert(diffuse_textures_filenames.size() == 3);
                assert(normalmap_textures_filenames.size() == 3);
                
                if(mask_texture_filename.length() != 0)
                {
                    texture_shared_ptr texture = m_fabricator->get_resource_accessor()->get_texture(mask_texture_filename, true);
                    assert(texture != nullptr);
                    texture->set_wrap_mode(GL_CLAMP_TO_EDGE);
                    texture->set_mag_filter(GL_LINEAR);
                    texture->set_min_filter(GL_LINEAR);
                    terrain->set_mask_texture(texture);
                }
                
                std::vector<texture_shared_ptr> diffuse_textures;
                for(i32 i = 0; i < 3; ++i)
                {
                    texture_shared_ptr texture = m_fabricator->get_resource_accessor()->get_texture(diffuse_textures_filenames[i], true);
                    assert(texture != nullptr);
                    texture->set_wrap_mode(GL_REPEAT);
                    texture->set_mag_filter(GL_LINEAR);
                    texture->set_min_filter(GL_LINEAR);
                    diffuse_textures.push_back(texture);
                }
                terrain->set_diffuse_textures(diffuse_textures);
                
                std::vector<texture_shared_ptr> normalmap_textures;
                for(i32 i = 0; i < 3; ++i)
                {
                    texture_shared_ptr texture = m_fabricator->get_resource_accessor()->get_texture(normalmap_textures_filenames[i], true);
                    assert(texture != nullptr);
                    texture->set_wrap_mode(GL_REPEAT);
                    texture->set_mag_filter(GL_LINEAR);
                    texture->set_min_filter(GL_LINEAR);
                    normalmap_textures.push_back(texture);
                }
                terrain->set_normalmap_textures(normalmap_textures);
                
                terrain->generate();
                
                m_game_objects_container.insert(terrain);
            }
            return terrain;
        }
    };
};

