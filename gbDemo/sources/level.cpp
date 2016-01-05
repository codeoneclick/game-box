//
//  level.cpp
//  gbDemo
//
//  Created by sergey.sergeev on 10/6/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "level.h"
#include "scene_fabricator.h"
#include "scene_graph.h"
#include "instanced_models3d_static.h"
#include "model3d_static.h"
#include "std_extensions.h"

namespace koth
{
    level::level(const gb::scene_fabricator_shared_ptr& fabricator,
                 const gb::scene_graph_shared_ptr& graph) :
    m_fabricator(fabricator),
    m_graph(graph)
    {
        m_size = glm::ivec2(16);
        m_boxes_size = glm::vec2(.85f);
        m_boxes_offset = .15f;
    }
    
    level::~level()
    {
        
    }
    
    void level::construct(const std::string& filename)
    {
        m_graph->set_box2d_world(glm::vec2(64.f), glm::vec2(128.f));
        
        m_boxes.resize(m_size.x * m_size.y, nullptr);
        m_boxes_states.resize(m_size.x * m_size.y, e_level_box_state_none);
        m_boxes_under_strain.resize(m_size.x * m_size.y, 0.f);
        m_navigation_map.resize(m_size.x * m_size.y, nullptr);
        
        std::string batch_guid = std::get_guid();
        
        for(i32 i = 0; i < m_size.x; ++i)
        {
            for (i32 j = 0; j < m_size.y; ++j)
            {
                m_boxes[i + j * m_size.x] = m_fabricator->create_model3d_static("gameobject.box.xml");
                //m_graph->add_game_object(m_boxes[i + j * m_size.x]);
                
                m_boxes[i + j * m_size.x]->set_is_batched(true, batch_guid);
                
                m_boxes[i + j * m_size.x]->set_position(glm::vec3(i * (m_boxes_size.x + m_boxes_offset), 0.f, j * (m_boxes_size.y + m_boxes_offset)));
                m_boxes[i + j * m_size.x]->set_scale(glm::vec3(m_boxes_size.x));
                
                m_navigation_map[i + j * m_size.x] = std::make_shared<level_node>();
                m_navigation_map[i + j * m_size.x]->set_position(i, j);
                m_navigation_map[i + j * m_size.x]->set_passable(true);
            }
        }
        level::update_navigation_map();
    }
    
    void level::update(f32 deltatime)
    {
        for(i32 i = 0; i < m_size.x; ++i)
        {
            for(i32 j = 0; j < m_size.y; ++j)
            {
                switch (m_boxes_states[i + j * m_size.x])
                {
                    case e_level_box_state_fall_down:
                    {
                        glm::vec3 position = m_boxes[i + j * m_size.x]->get_position();
                        glm::vec3 rotation = m_boxes[i + j * m_size.x]->get_rotation();
                        
                        if(position.y > -24.f)
                        {
                            position.y -= .05f;
                            rotation.x += 2.5f;
                            rotation.z += .5f;
                        }
                        else
                        {
                            m_boxes_states[i + j * m_size.x] = e_level_box_state_drop_down;
                            position.y = 24.f;
                            rotation = glm::vec3(0.f);
                        }
                        
                        m_boxes[i + j * m_size.x]->set_position(position);
                        m_boxes[i + j * m_size.x]->set_rotation(rotation);
                        
                        m_navigation_map[i + j * m_size.x]->set_passable(false);
                    }
                        break;
                        
                    case e_level_box_state_drop_down:
                    {
                        glm::vec3 position = m_boxes[i + j * m_size.x]->get_position();
                        glm::vec3 rotation = m_boxes[i + j * m_size.x]->get_rotation();
                        
                        if(position.y > 0.f)
                        {
                            position.y -= .1f;
                        }
                        else
                        {
                            m_boxes_states[i + j * m_size.x] = e_level_box_state_none;
                        }
                        
                        m_boxes[i + j * m_size.x]->set_position(position);
                        m_boxes[i + j * m_size.x]->set_rotation(rotation);
                    }
                        break;
                        
                    case e_level_box_state_none:
                    {
                        m_navigation_map[i + j * m_size.x]->set_passable(true);
                        glm::vec3 position = m_boxes[i + j * m_size.x]->get_position();
                        if(m_boxes_under_strain[i + j * m_size.x] < position.y)
                        {
                            position.y -= .01f;
                            
                        }
                        else if(m_boxes_under_strain[i + j * m_size.x] > position.y)
                        {
                            position.y += .01f;
                        }
                        
                        m_boxes[i + j * m_size.x]->set_position(position);
                        /*static f32 time = 0.f;
                        time += .0001f;
                        
                        position.y = sinf(.5f * position.x + time) * cosf(.5f * position.z + time) * .25f;
                        m_boxes[i + j * m_size.x]->set_position(position);*/
                    }
                        break;
                        
                    default:
                        break;
                }
            }
        }
        level::update_navigation_map();
    }
    
    void level::set_box_state(i32 x, i32 z)
    {
        if(x < m_size.x && z < m_size.y &&
           x >= 0 && z >= 0 &&
           m_boxes_states[x + z * m_size.x] == e_level_box_state_none)
        {
            m_boxes_states[x + z * m_size.x] = e_level_box_state_fall_down;
        }
    }
    
    void level::set_box_under_strain(f32 x, f32 y)
    {
        for(i32 i = 0; i < m_size.x; ++i)
        {
            for(i32 j = 0; j < m_size.y; ++j)
            {
                f32 distance = glm::distance(glm::vec2(x, y), glm::vec2(i, j));
                distance /= 3.f;
                
                f32 value = glm::mix(-.4f, 0.f, std::min(distance, 1.f));
                m_boxes_under_strain[i + j * m_size.x] = m_boxes_under_strain[i + j * m_size.x] > value ? value : m_boxes_under_strain[i + j * m_size.x];
            }
        }
    }
    
    void level::cleanup()
    {
        for(i32 x = 0; x < m_size.x; ++x)
        {
            for(i32 y = 0; y < m_size.y; ++y)
            {
                 m_boxes_under_strain[x + y * m_size.x] = 0.f;
            }
        }
    }
    
    void level::update_navigation_map()
    {
        std::shared_ptr<level_node> current_child = nullptr;
        for(i32 x = 0; x < m_size.x; ++x)
        {
            for(i32 y = 0; y < m_size.y; ++y)
            {
                m_navigation_map[x + y * m_size.x]->remove_children();
                
                for(i32 i = -1; i < 2; ++i)
                {
                    i32 new_x = m_navigation_map[x + y * m_size.x]->get_x() + i;
                    for(i32 j = -1; j < 2; ++j)
                    {
                        i32 new_y = m_navigation_map[x + y * m_size.x]->get_y() + j;
                        if(new_x > -1 && new_x < m_size.x && new_y > -1 && new_y < m_size.y)
                        {
                            current_child = m_navigation_map[new_x + new_y * m_size.x];
                            if(current_child->get_passable() && (new_x != x || new_y != y))
                            {
                                m_navigation_map[x + y * m_size.x]->add_child(current_child,
                                                                              m_navigation_map[x + y * m_size.x]->distance_to_local(current_child));
                            }
                        }
                    }
                }
            }
        }
    }
    
    level_node_shared_ptr level::get_path_node(i32 x, i32 y)
    {
        level_node_shared_ptr path_node = nullptr;
        if(x > -1 && x < m_size.x && y > -1 && y < m_size.y)
        {
            path_node = m_navigation_map[x + y * m_size.x];
        }
        return path_node;
    }
    
    glm::ivec2 level::get_size() const
    {
        return m_size;
    }
}