//
//  content_list.cpp
//  gbUI
//
//  Created by sergey.sergeev on 1/15/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "content_list.h"
#include "scene_fabricator.h"
#include "sprite.h"
#include "ces_bound_touch_component.h"

namespace gb
{
    namespace ui
    {
        content_list::content_list(const scene_fabricator_shared_ptr& fabricator) :
        gb::ui::control(fabricator),
        m_create_cell_callback(nullptr)
        {
            ces_bound_touch_component_shared_ptr bound_touch_compoent = std::make_shared<ces_bound_touch_component>();
            ces_entity::add_component(bound_touch_compoent);
        }
        
        content_list::~content_list()
        {
            
        }
        
        void content_list::create()
        {
            gb::sprite_shared_ptr content_list_background = control::get_fabricator()->create_sprite("content_list_background_background.xml");
            m_elements["content_list_background_background"] = content_list_background;
            ces_entity::add_child(content_list_background);
        }
        
        void content_list::on_touched(const ces_entity_shared_ptr&, const glm::vec2& point, e_input_element input_element, e_input_state input_state)
        {

        }
        
        void content_list::set_size(const glm::vec2& size)
        {
            m_size = size;
            unsafe_get_bound_touch_component_from_this->set_frame(glm::vec4(0.f, 0.f, m_size.x, m_size.y));
        }
        
        glm::vec2 content_list::get_size() const
        {
            return m_size;
        }
        
        void content_list::set_create_cell_callback(const t_create_cell_callback& callback)
        {
            
        }
        
        void content_list::set_data_source(const std::vector<content_list_data>& data_source)
        {
            
        }
    }
}