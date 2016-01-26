//
//  content_list_cell.cpp
//  gbUI
//
//  Created by sergey.sergeev on 1/22/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "content_list_cell.h"
#include "scene_fabricator.h"
#include "sprite.h"
#include "ces_transformation_component.h"
#include "ces_bound_touch_component.h"
#include "ces_material_component.h"

namespace gb
{
    namespace ui
    {
        static const std::string k_color_state_uniform = "u_color";
        
        content_list_cell::content_list_cell(const scene_fabricator_shared_ptr& fabricator, i32 data_source_index, const std::string& identifier) :
        gb::ui::control(fabricator),
        m_data_source_index(data_source_index),
        m_identifier(identifier)
        {
            ces_bound_touch_component_shared_ptr bound_touch_compoent = std::make_shared<ces_bound_touch_component>();
            ces_entity::add_component(bound_touch_compoent);
        }
        
        content_list_cell::~content_list_cell()
        {
            
        }
        
        void content_list_cell::create()
        {
            gb::sprite_shared_ptr content_list_cell_background =
            control::get_fabricator()->create_sprite("content_list_cell_background.xml");
            m_elements["content_list_cell_background"] = content_list_cell_background;
            ces_entity::add_child(content_list_cell_background);
            
            ces_material_component* material_component = unsafe_get_material_component(content_list_cell_background);
            material_component->set_custom_shader_uniform(control::k_gray_color, k_color_state_uniform);
            
            control::create();
        }
        
        void content_list_cell::on_touched(const ces_entity_shared_ptr&, const glm::vec2& point,
                                           e_input_element input_element, e_input_state input_state)
        {
            
        }
        
        void content_list_cell::set_size(const glm::vec2& size)
        {
            control::set_size(size);
            
            unsafe_get_bound_touch_component_from_this->set_frame(glm::vec4(0.f, 0.f, m_size.x, m_size.y));
            
            std::static_pointer_cast<gb::sprite>(m_elements["content_list_cell_background"])->set_size(size);
        }
        
        i32 content_list_cell::get_data_source_index() const
        {
            return m_data_source_index;
        }
        
        std::string content_list_cell::get_identifier() const
        {
            return m_identifier;
        }
        
        void content_list_cell::set_data_source_index(i32 index)
        {
            m_data_source_index = index;
        }
    };
};