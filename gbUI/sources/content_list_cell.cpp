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

namespace gb
{
    namespace ui
    {
        content_list_cell::content_list_cell(const scene_fabricator_shared_ptr& fabricator) :
        gb::ui::control(fabricator)
        {
            
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
            
            control::create();
        }
        
        void content_list_cell::on_touched(const ces_entity_shared_ptr&, const glm::vec2& point,
                                           e_input_element input_element, e_input_state input_state)
        {
            
        }
    };
};