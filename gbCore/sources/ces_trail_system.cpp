//
//  ces_trail_system.cpp
//  gbCore
//
//  Created by serhii.s on 3/28/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_trail_system.h"
#include "ces_trail_component.h"
#include "ces_geometry_3d_component.h"
#include "mesh_3d.h"

namespace gb
{
    ces_trail_system::ces_trail_system()
    {
        ces_base_system::add_required_component_guid(m_components_mask, ces_trail_component::class_guid());
        ces_base_system::add_required_components_mask(m_components_mask);
    }
    
    void ces_trail_system::on_feed_start(f32 dt)
    {
        
    }
    
    void ces_trail_system::on_feed(const ces_entity_shared_ptr& root, f32 dt)
    {
        ces_base_system::enumerate_entities_with_components(m_components_mask, [=](const ces_entity_shared_ptr& entity) {
            const auto trail_component = entity->get_component<ces_trail_component>();
            const auto geometry_component = entity->get_component<ces_geometry_3d_component>();
            const auto mesh = geometry_component->get_mesh();
            vbo::vertex_attribute_PTNTC *vertices = mesh->get_vbo()->lock<vbo::vertex_attribute_PTNTC>();
            const auto old_segment_position = trail_component->get_last_segment_position();
            const auto new_segment_position = trail_component->get_new_segment_position();
            const auto segment_width = trail_component->get_width();
            
            const auto source_direction = glm::normalize(trail_component->get_start_segment_direction());
            const auto destination_direction = glm::cross(glm::vec3(0.f, 1.f, 0.f), source_direction);
            
            auto used_segments_num = trail_component->get_used_segments_num();
            if (used_segments_num == 0)
            {
                vertices[0].m_position = old_segment_position + destination_direction * segment_width;
                vertices[0].m_texcoord = glm::packUnorm2x16(glm::vec2(0.f, 0.f));

                vertices[1].m_position = old_segment_position - destination_direction * segment_width;
                vertices[1].m_texcoord = glm::packUnorm2x16(glm::vec2(0.f, 1.f));

                vertices[2].m_position = old_segment_position + destination_direction * segment_width;
                vertices[2].m_texcoord = glm::packUnorm2x16(glm::vec2(1.f, 0.f));

                vertices[3].m_position = old_segment_position - destination_direction * segment_width;
                vertices[3].m_texcoord = glm::packUnorm2x16(glm::vec2(1.f, 1.f));
                
                used_segments_num = 1;
            }
            
            f32 current_segment_length = glm::length(new_segment_position - old_segment_position);
            
            if (current_segment_length > trail_component->get_segment_length())
            {
                ui32 current_used_segments_num = used_segments_num;
                if (current_used_segments_num >= trail_component->get_segments_num() - 1)
                {
                    for (ui32 i = 0; i < trail_component->get_segments_num() - 2; ++i)
                    {
                        vertices[i * 4] = vertices[i * 4 + 4];
                        vertices[i * 4 + 1] = vertices[i * 4 + 5];
                        vertices[i * 4 + 2] = vertices[i * 4 + 6];
                        vertices[i * 4 + 3] = vertices[i * 4 + 7];
                    }
                    current_used_segments_num--;
                }
                else
                {
                    used_segments_num++;
                }
                
                vertices[current_used_segments_num * 4].m_position = vertices[(current_used_segments_num - 1) * 4 + 2].m_position;
                vertices[current_used_segments_num * 4].m_texcoord = glm::packUnorm2x16(glm::vec2(0.f, 0.f));
                
                vertices[current_used_segments_num * 4 + 1].m_position = vertices[(current_used_segments_num - 1) * 4 + 3].m_position;
                vertices[current_used_segments_num * 4 + 1].m_texcoord = glm::packUnorm2x16(glm::vec2(0.f, 1.f));
                
                vertices[current_used_segments_num * 4 + 2].m_position = new_segment_position + destination_direction * segment_width;
                vertices[current_used_segments_num * 4 + 2].m_texcoord = glm::packUnorm2x16(glm::vec2(1.f, 0.f));
                
                vertices[current_used_segments_num * 4 + 3].m_position = new_segment_position - destination_direction * segment_width;
                vertices[current_used_segments_num * 4 + 3].m_texcoord = glm::packUnorm2x16(glm::vec2(1.f, 1.f));
                
                trail_component->set_last_segment_position(new_segment_position);
            }
            else if (current_segment_length > trail_component->get_min_segment_length())
            {
                ui32 current_used_segments_num = used_segments_num - 1;
                
                //vertices[current_used_segments_num * 4].m_position = old_segment_position + destination_direction * segment_width;
                //vertices[current_used_segments_num * 4 + 1].m_position = old_segment_position - destination_direction * segment_width;
                vertices[current_used_segments_num * 4 + 2].m_position = new_segment_position + destination_direction * segment_width;
                vertices[current_used_segments_num * 4 + 3].m_position = new_segment_position - destination_direction * segment_width;
                
                /*if (current_used_segments_num >= 2)
                {
                    auto old_direction = vertices[(current_used_segments_num - 1) * 4].m_position -
                    vertices[(current_used_segments_num - 2) * 4].m_position;
                    
                    old_direction = glm::cross(glm::vec3(0.f, 1.f, 0.f), glm::normalize(old_direction));
                    old_direction = old_direction + (1.f - glm::dot(old_direction, direction)) * direction;
                    old_direction = glm::normalize(old_direction);
                    
                    vertices[(current_used_segments_num - 1) * 4 + 2].m_position = old_segment_position + old_direction * segment_width;
                    vertices[(current_used_segments_num - 1) * 4 + 3].m_position = old_segment_position - old_direction * segment_width;
                }*/
            }
            mesh->get_vbo()->unlock();
            mesh->get_ibo()->unlock(used_segments_num * 6);
            
            trail_component->set_used_segments_num(used_segments_num);
        });
    }
    
    void ces_trail_system::on_feed_end(f32 dt)
    {
        
    }
}
