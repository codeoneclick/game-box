//
//  content_tab_list_cell.cpp
//  gbUI
//
//  Created by sergey.sergeev on 1/25/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "content_tab_list_cell.h"
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
        
        content_tab_list_cell::content_tab_list_cell(const scene_fabricator_shared_ptr& fabricator) :
        gb::ui::control(fabricator)
        {

        }
        
        content_tab_list_cell::~content_tab_list_cell()
        {
            
        }
        
        void content_tab_list_cell::create()
        {
            gb::sprite_shared_ptr content_tab_list_cell_background =
            control::get_fabricator()->create_sprite("content_tab_list_cell_background.xml");
            m_elements["content_tab_list_cell_background"] = content_tab_list_cell_background;
            ces_entity::add_child(content_tab_list_cell_background);
            
            ces_material_component* material_component = unsafe_get_material_component(content_tab_list_cell_background);
            material_component->set_custom_shader_uniform(control::k_gray_color, k_color_state_uniform);
            
            control::create();
        }
        
        void content_tab_list_cell::on_touched(const ces_entity_shared_ptr&, const glm::vec2& point,
                                               e_input_source input_source, e_input_state input_state)
        {
            
        }
        
        void content_tab_list_cell::set_size(const glm::vec2& size)
        {
            control::set_size(size);
            std::static_pointer_cast<gb::sprite>(m_elements["content_tab_list_cell_background"])->size = size;
        }
    };
};