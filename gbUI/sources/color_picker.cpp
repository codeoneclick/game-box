//
//  color_picker.cpp
//  gbUI
//
//  Created by serhii.s on 8/10/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "color_picker.h"
#include "scene_fabricator.h"
#include "sprite.h"
#include "ces_bound_touch_2d_component.h"
#include "ces_material_component.h"
#include "ces_transformation_extension.h"
#include "game_command.h"
#include "input_context.h"
#include "glm_extensions.h"
#include "ces_sound_component.h"
#include "texture.h"

namespace gb
{
    namespace ui
    {
        std::string color_picker::k_color_wheel_element_name = "color_wheel_element";
        
        color_picker::color_picker(const scene_fabricator_shared_ptr& fabricator) :
        gb::ui::interaction_control(fabricator)
        {
            ces_entity::add_deferred_component_constructor<ces_bound_touch_2d_component>();
            
            size.setter([=](const glm::vec2& size) {
                
                m_size = size;
                interaction_control::on_touch_size_changed(m_size);
                
                std::static_pointer_cast<gb::sprite>(m_elements[k_background_element_name])->size = size;
                std::static_pointer_cast<gb::sprite>(m_elements[k_color_wheel_element_name])->size = size;
            });
        }
        
        void color_picker::setup_components()
        {
            interaction_control::setup_components();
        }
        
        void color_picker::create()
        {
            const auto background = control::get_fabricator()->create_sprite("button_background.xml");
            m_elements[k_background_element_name] = background;
            add_child(background);
            
            const auto color_wheel_texture = texture::construct("color_wheel_texture", 256, 256, gl::constant::rgba_t, nullptr);
            
            control::get_fabricator()->get_resource_accessor()->add_custom_resource("color_wheel_texture", color_wheel_texture);
            const auto color_wheel = control::get_fabricator()->create_sprite("button_image.xml", "color_wheel_texture");
            m_elements[k_color_wheel_element_name] = color_wheel;
            add_child(color_wheel);
            
            interaction_control::create();
        }
        
        void color_picker::on_touched(const ces_entity_shared_ptr& entity,
                                      const glm::vec2& touch_point,
                                      e_input_source input_source,
                                      e_input_state input_state)
        {
            interaction_control::on_touched(entity, touch_point, input_source, input_state);
            auto bound_touch_component = ces_entity::get_component<ces_bound_touch_component>();
            if(input_state == e_input_state_pressed)
            {
               
            }
            else if(input_state == e_input_state_released)
            {
            
            }
        }
        
        void color_picker::on_dragging(const ces_entity_shared_ptr& entity,
                                       const glm::vec2& touch_point,
                                       e_input_source input_source,
                                       e_input_state input_state)
        {
            interaction_control::on_dragging(entity, touch_point, input_source, input_state);
            
            glm::vec2 size = control::size;
            glm::mat4 mat_m = ces_transformation_extension::get_absolute_transformation_in_ws(shared_from_this());
            glm::vec2 min_bound = glm::transform(glm::vec2(0.f),
                                                 mat_m);
            glm::vec2 max_bound = glm::transform(size,
                                                 mat_m);
            glm::vec4 bound = glm::vec4(min_bound.x, min_bound.y, max_bound.x, max_bound.y);
            
            if(glm::intersect(bound, touch_point))
            {
                
            }
            
        }
    }
}
