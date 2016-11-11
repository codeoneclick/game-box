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
#include "brush.h"
#include "landscape.h"
#include "canvas.h"
#include "resource_accessor.h"
#include "mesh.h"
#include "material.h"
#include "mesh_constructor.h"
#include "texture_configuration.h"
#include "sprite_configuration.h"
#include "configuration_accessor.h"
#include "ces_geometry_freeform_component.h"
#include "ces_convex_hull_component.h"
#include "ces_shadow_component.h"
#include "texture_loading_operation.h"

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
                
                glm::vec4 grid_bound = grid->bound;
                
                vbo::vertex_attribute vertices[4];
                vertices[0].m_position = glm::vec3(grid_bound.x, grid_bound.y, 0.f);
                vertices[1].m_position = glm::vec3(grid_bound.z, grid_bound.y, 0.f);
                vertices[2].m_position = glm::vec3(grid_bound.z, grid_bound.w, 0.f);
                vertices[3].m_position = glm::vec3(grid_bound.x, grid_bound.w, 0.f);
                
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
        
        landscape_shared_ptr ed_fabricator::create_landscape(const std::string& filename, const glm::vec2& size,
                                                             const std::vector<std::string>& masks_filenames,
                                                             const std::vector<std::string>& brushes_filenames)
        {
            std::shared_ptr<sprite_configuration> landscape_configuration =
            std::static_pointer_cast<gb::sprite_configuration>(m_fabricator->get_configuration_accessor()->get_sprite_configuration(filename));
            assert(landscape_configuration);
            landscape_shared_ptr landscape = nullptr;
            if(landscape_configuration)
            {
                landscape = std::make_shared<gb::ed::landscape>();
                m_fabricator->add_materials(landscape, landscape_configuration->get_materials_configurations());
                
                landscape->size = size;
                
                for(i32 i = 0; i < masks_filenames.size(); ++i)
                {
                    texture_shared_ptr texture = m_fabricator->get_resource_accessor()->get_resource<gb::texture, gb::texture_loading_operation>(masks_filenames[i], true);
                    assert(texture != nullptr);
                    texture->set_wrap_mode(GL_CLAMP_TO_EDGE);
                    texture->set_mag_filter(GL_LINEAR);
                    texture->set_min_filter(GL_LINEAR);
                    landscape->add_mask(texture);
                }
                
                i32 layer = 0;
                i32 previous_layer = 0;
                i32 sampler = 0;
                for(i32 i = 0; i < brushes_filenames.size(); ++i)
                {
                    texture_shared_ptr texture = m_fabricator->get_resource_accessor()->get_resource<gb::texture, gb::texture_loading_operation>(brushes_filenames[i], true);
                    assert(texture != nullptr);
                    texture->set_wrap_mode(GL_REPEAT);
                    texture->set_mag_filter(GL_LINEAR);
                    texture->set_min_filter(GL_LINEAR);
                    landscape->add_brush(texture, nullptr, layer, sampler);
                    
                    previous_layer = layer;
                    layer = i > 2 ? (i - 2) / 2 + 1 : layer == 2 ? 1 : 0;
                    sampler = previous_layer != layer ? 0 : sampler + 1;
                }
                
                landscape->generate();
                m_game_objects_container.insert(landscape);
            }
            return landscape;
        }
        
        brush_shared_ptr ed_fabricator::create_brush(const std::string& filename)
        {
            std::shared_ptr<sprite_configuration> brush_configuration =
            std::static_pointer_cast<gb::sprite_configuration>(m_fabricator->get_configuration_accessor()->get_sprite_configuration(filename));
            assert(brush_configuration);
            brush_shared_ptr brush = nullptr;
            if(brush_configuration)
            {
                brush = std::make_shared<gb::ed::brush>();
                
                m_fabricator->add_materials(brush, brush_configuration->get_materials_configurations());
                
                m_game_objects_container.insert(brush);
            }
            return brush;
        }
        
        canvas_shared_ptr ed_fabricator::create_canvas(const std::string& filename)
        {
            std::shared_ptr<sprite_configuration> canvas_configuration =
            std::static_pointer_cast<gb::sprite_configuration>(m_fabricator->get_configuration_accessor()->get_sprite_configuration(filename));
            assert(canvas_configuration);
            canvas_shared_ptr canvas = nullptr;
            if(canvas_configuration)
            {
                canvas = std::make_shared<gb::ed::canvas>();
                
                for(const auto& iterator : canvas_configuration->get_materials_configurations())
                {
                    std::shared_ptr<material_configuration> material_configuration =
                    std::static_pointer_cast<gb::material_configuration>(iterator);
                    
                    material_shared_ptr material = material::construct(material_configuration);
                    gb::material::set_shader(material, material_configuration, m_fabricator->get_resource_accessor());
                    gb::material::set_textures(material, material_configuration, m_fabricator->get_resource_accessor());
                    canvas->add_material(material_configuration->get_technique_name(),
                                         material_configuration->get_technique_pass(), material);
                }
                m_game_objects_container.insert(canvas);
            }
            return canvas;
        }
        
        void ed_fabricator::add_texture_to_brush(const brush_shared_ptr& brush, const std::string& filename)
        {
            texture_shared_ptr texture = m_fabricator->get_resource_accessor()->get_resource<gb::texture, gb::texture_loading_operation>(filename, true);
            assert(texture != nullptr);
            texture->set_wrap_mode(GL_REPEAT);
            texture->set_mag_filter(GL_LINEAR);
            texture->set_min_filter(GL_LINEAR);
            brush->add_texture(texture);
        }
    };
};

