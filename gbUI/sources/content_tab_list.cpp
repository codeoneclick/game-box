//
//  content_tab_list.cpp
//  gbUI
//
//  Created by sergey.sergeev on 1/25/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "content_tab_list.h"
#include "scene_fabricator.h"
#include "sprite.h"
#include "ces_bound_touch_component.h"
#include "content_tab_list_cell.h"
#include "ces_transformation_component.h"
#include "ces_action_component.h"
#include "ces_material_component.h"
#include "button.h"

namespace gb
{
    namespace ui
    {
        static const std::string k_color_state_uniform = "u_color";
     
        content_tab_list_data::content_tab_list_data(const std::string& label) :
        m_label(label)
        {
            
        }
        
        std::string content_tab_list_data::get_label() const
        {
            return m_label;
        }
        
        content_tab_list::content_tab_list(const scene_fabricator_shared_ptr& fabricator) :
        gb::ui::control(fabricator),
        m_on_create_cell_callback(nullptr),
        m_separator_offset(glm::vec2(10.f))
        {
            size.setter([=](const glm::vec2& size) {
                
                m_size = size;
                m_elements["content_tab_list_background"]->size = glm::vec2(size.x + m_separator_offset.x * 2.f, size.y);
                
            });
        }
        
        content_tab_list::~content_tab_list()
        {
            
        }
        
        void content_tab_list::create()
        {
            gb::sprite_shared_ptr content_tab_list_background =
            control::get_fabricator()->create_sprite("content_tab_list_background.xml");
            m_elements["content_tab_list_background"] = content_tab_list_background;
            game_object::add_child(content_tab_list_background);
            
            auto material_component = content_tab_list_background->get_unsafe_component<ces_material_component>();
            material_component->set_custom_shader_uniform(control::k_dark_gray_color, k_color_state_uniform);
            
            control::create();
        }
        
        void content_tab_list::set_separator_offset(const glm::vec2& separator_offset)
        {
            m_separator_offset = separator_offset;
        }
        
        void content_tab_list::set_on_create_cell_callback(const t_on_create_cell_callback& callback)
        {
            m_on_create_cell_callback = callback;
        }
        
        void content_tab_list::set_data_source(const std::vector<content_tab_list_data_shared_ptr>& data_source)
        {
            assert(m_on_create_cell_callback);
            
            f32 offset_x = 0.f;
            f32 size_x = (m_size.x - m_separator_offset.x * (static_cast<i32>(data_source.size()) - 3)) / data_source.size();
            for(i32 i = 0; i < data_source.size(); ++i)
            {
                button_shared_ptr tab = std::make_shared<button>(control::get_fabricator());
                tab->create();
                tab->size = glm::vec2(size_x, m_size.y);
                tab->position = glm::vec2(offset_x, 0.f);
                tab->set_text(data_source[i]->get_label());
                tab->set_on_pressed_callback(std::bind(&content_tab_list::on_tab_pressed, this, std::placeholders::_1));
                game_object::add_child(tab);
                m_tabs.push_back(tab);
                
                offset_x += size_x + m_separator_offset.x;

                content_tab_list_cell_shared_ptr cell = m_on_create_cell_callback(i, data_source[i]);
                cell->position = glm::vec2(0.f, m_separator_offset.y + m_size.y);
                game_object::add_child(cell);
                m_cells.push_back(cell);
                
                cell->visible = i == 0;
            }
        }
        
        void content_tab_list::on_tab_pressed(const ces_entity_shared_ptr& entity)
        {
            for (i32 i = 0; i < m_tabs.size(); ++i)
            {
                m_cells[i]->visible = false;
                if(entity == m_tabs[i])
                {
                    m_cells[i]->visible = true;
                }
            }
        }
    }
}
