//
//  characters_3d_controller.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 5/19/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "characters_3d_controller.h"
#include "camera_3d.h"
#include "scene_graph.h"
#include "scene_fabricator.h"
#include "shape_3d.h"
#include "sprite.h"

namespace game
{
    const f32 characters_3d_controller::k_viewport_width = 16;
    const f32 characters_3d_controller::k_viewport_height = 16;
    
    const i32 characters_3d_controller::k_slots_cols = 4;
    const i32 characters_3d_controller::k_slots_rows = 4;

    
    characters_3d_controller::characters_3d_controller(const gb::scene_graph_shared_ptr& scene_graph,
                                                       const gb::scene_fabricator_shared_ptr& scene_fabricator) :
    m_scene_graph(scene_graph),
    m_scene_fabricator(scene_fabricator)
    {
        m_camera_3d = std::make_shared<gb::camera_3d>(90.f, .1f, 1024.f,
                                                      glm::ivec4(0.f,
                                                                 0.f,
                                                                 k_viewport_width,
                                                                 k_viewport_height), false);
        m_camera_3d->set_distance_to_look_at(glm::vec3(8.f, 0.f, 0.f));
        scene_graph->set_camera_3d(m_camera_3d);
        
        m_used_slots_cols.fill(0);
        m_used_slots_rows.fill(0);
    }
    
    characters_3d_controller::~characters_3d_controller()
    {
        
    }
    
    std::pair<gb::sprite_shared_ptr, gb::shape_3d_shared_ptr> characters_3d_controller::create_character(const std::string& character_sprite_filename,
                                                                                                         const std::string& character_shape_3d_filename,
                                                                                                         const glm::vec2& size,
                                                                                                         e_view_type view_type)
    {
        auto shape_3d = m_scene_fabricator.lock()->create_shape_3d(character_shape_3d_filename);
        m_scene_graph.lock()->add_child(shape_3d);
        
        f32 scale = .025f;
        
        f32 position_offset_x = (k_viewport_width / static_cast<f32>(k_slots_cols));
        f32 position_offset_y = (k_viewport_width / static_cast<f32>(k_slots_rows));
        
        f32 offset_to_center_x = position_offset_x * .5f;
        f32 offset_to_center_y = position_offset_y * .5f;
        
        position_offset_x = m_used_slots_rows[view_type] * position_offset_x + offset_to_center_x;
        position_offset_y = m_used_slots_cols[view_type] * position_offset_y + offset_to_center_y;
        
        f32 texcoord_step_x = 1.f / static_cast<f32>(k_slots_cols);
        f32 texcoord_step_y = 1.f / static_cast<f32>(k_slots_rows);
        
        f32 texcoord_offset_x = texcoord_step_x * m_used_slots_cols[view_type];
        f32 texcoord_offset_y = 1.f - texcoord_step_y * m_used_slots_rows[view_type];
        
        m_used_slots_cols[view_type]++;
        if(m_used_slots_cols[view_type] >= k_slots_cols)
        {
            m_used_slots_cols[view_type] = 0;
            m_used_slots_rows[view_type]++;
            if(m_used_slots_rows[view_type] >= k_slots_rows)
            {
                assert(false);
            }
        }
        
        shape_3d->position = glm::vec3(0.f, position_offset_x, position_offset_y);
        shape_3d->scale = glm::vec3(scale);
        shape_3d->play_animation("idle");
        shape_3d->rotation = glm::vec3(0.f, -90.f, view_type == e_view_type_top ? 90.f : 0.f);
        
        auto sprite = m_scene_fabricator.lock()->create_sprite(character_sprite_filename);
        sprite->size = size;
        sprite->pivot = glm::vec2(.5f);
        sprite->set_custom_texcoord(glm::vec4(texcoord_offset_x,
                                              texcoord_offset_y - texcoord_step_y,
                                              texcoord_offset_x + texcoord_step_x,
                                              texcoord_offset_y));
        sprite->rotation = view_type == e_view_type_top ? 180.f : 0.f;
        sprite->tag = "body";
        
        auto characters_linkage = std::make_pair(sprite, shape_3d);
        m_characters_3d_container.insert(characters_linkage);
        return characters_linkage;
    }
    
    void characters_3d_controller::remove_character(const gb::ces_entity_shared_ptr& character)
    {
        auto characters_linkage = m_characters_3d_container.find(character);
        if(characters_linkage != m_characters_3d_container.end())
        {
            m_characters_3d_container.erase(characters_linkage);
        }
    }
}
