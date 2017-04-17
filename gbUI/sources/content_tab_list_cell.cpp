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
        content_tab_list_cell::content_tab_list_cell(const scene_fabricator_shared_ptr& fabricator) :
        gb::ui::control(fabricator)
        {
            size.setter([=](const glm::vec2& size) {
                
                m_size = size;
                m_elements["content_tab_list_cell_background"]->size = size;
                
            });
        }
        
        content_tab_list_cell::~content_tab_list_cell()
        {
            
        }
        
        void content_tab_list_cell::create()
        {
            gb::sprite_shared_ptr content_tab_list_cell_background =
            control::get_fabricator()->create_sprite("content_tab_list_cell_background.xml");
            m_elements["content_tab_list_cell_background"] = content_tab_list_cell_background;
            game_object::add_child(content_tab_list_cell_background);
            
            control::create();
        }
        
        void content_tab_list_cell::on_touched(const ces_entity_shared_ptr&, const glm::vec2& point,
                                               e_input_source input_source, e_input_state input_state)
        {
            
        }
    };
};
