//
//  ces_ui_quest_dialog_component.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 8/8/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_ui_quest_dialog_component.h"
#include "button.h"
#include "textfield.h"
#include "ces_bound_touch_2d_component.h"

namespace game
{
    ces_ui_quest_dialog_component::quest_table_view_cell_data::quest_table_view_cell_data() :
    m_quest_id(-1)
    {
        
    }
    
    ces_ui_quest_dialog_component::quest_table_view_cell_data::~quest_table_view_cell_data()
    {
        
    }
    
    void ces_ui_quest_dialog_component::quest_table_view_cell_data::set_quest_id(i32 id)
    {
        m_quest_id = id;
    }
    
    i32 ces_ui_quest_dialog_component::quest_table_view_cell_data::get_quest_id() const
    {
        return m_quest_id;
    }
    
    const std::string ces_ui_quest_dialog_component::quest_table_view_cell::k_accept_quest_button = "accept_quest_button";
    const std::string ces_ui_quest_dialog_component::quest_table_view_cell::k_in_progress_quest_button = "in_progress_quest_button";
    
    ces_ui_quest_dialog_component::quest_table_view_cell::quest_table_view_cell(const gb::scene_fabricator_shared_ptr& fabricator, i32 index, const std::string& identifier) :
    gb::ui::table_view_cell(fabricator, index, identifier),
    m_accept_quest_button_callback(nullptr)
    {
        size.setter([=](const glm::vec2& size) {
            
            m_size = size;
            auto bound_touch_component = ces_entity::get_component<gb::ces_bound_touch_component>();
            bound_touch_component->as_2d()->set_bounds(glm::vec4(0.f, 0.f, m_size.x, m_size.y));
            m_elements[control::k_background_element_name]->size = size;
            
            m_elements[k_accept_quest_button]->position = glm::vec2(m_size.x - 128.f - 8.f, 8.f);
            m_elements[k_in_progress_quest_button]->position = glm::vec2(m_size.x - 128.f - 8.f, 8.f + 24.f + 16.f);
        });
    }
    
    ces_ui_quest_dialog_component::quest_table_view_cell::~quest_table_view_cell()
    {
        
    }
    
    void ces_ui_quest_dialog_component::quest_table_view_cell::setup_components()
    {
        gb::ui::table_view_cell::setup_components();
    }
    
    void ces_ui_quest_dialog_component::quest_table_view_cell::create()
    {
        gb::ui::table_view_cell::create();
        
        glm::vec2 cell_size = quest_table_view_cell::size;
        
        auto accept_quest_button = gb::ces_entity::construct<gb::ui::button>(control::get_fabricator());
        accept_quest_button->create();
        accept_quest_button->size = glm::vec2(128.f, 24.f);
        accept_quest_button->position = glm::vec2(cell_size.x - 128.f - 8.f, 8.f);
        accept_quest_button->set_text("Accept");
        m_elements[k_accept_quest_button] = accept_quest_button;
        quest_table_view_cell::add_child(accept_quest_button);
        
        auto quest_in_progress_textfield = gb::ces_entity::construct<gb::ui::textfield>(control::get_fabricator());
        quest_in_progress_textfield->create();
        quest_in_progress_textfield->size = glm::vec2(128.f, 24.f);
        quest_in_progress_textfield->set_text("In Progress");
        quest_in_progress_textfield->position = glm::vec2(cell_size.x - 128.f - 8.f, 8.f + 24.f + 16.f);
        m_elements[k_in_progress_quest_button] = quest_in_progress_textfield;
        quest_table_view_cell::add_child(quest_in_progress_textfield);
    }
    
    void ces_ui_quest_dialog_component::quest_table_view_cell::set_accept_quest_button_callback(const accept_quest_button_callback_t& callback)
    {
        m_accept_quest_button_callback = callback;
        if(!std::static_pointer_cast<gb::ui::button>(m_elements[k_accept_quest_button])->is_pressed_callback_exist())
        {
            std::static_pointer_cast<gb::ui::button>(m_elements[k_accept_quest_button])->set_on_pressed_callback([this](const gb::ces_entity_shared_ptr&) {
                if(m_accept_quest_button_callback)
                {
                    m_accept_quest_button_callback(m_index);
                }
            });
        }
    }
    
    void ces_ui_quest_dialog_component::quest_table_view_cell::set_quest_in_progress(bool value)
    {
        m_elements[k_accept_quest_button]->visible = !value;
        m_elements[k_in_progress_quest_button]->visible = value;
    }

    ces_ui_quest_dialog_component::ces_ui_quest_dialog_component() :
    m_selected_quest_id(-1)
    {
        
    }
    
    void ces_ui_quest_dialog_component::set_selected_quest_id(i32 id)
    {
        m_selected_quest_id = id;
    }
    
    i32 ces_ui_quest_dialog_component::get_selected_quest_id() const
    {
        return m_selected_quest_id;
    }
    
    bool ces_ui_quest_dialog_component::is_selected_quest_id_exist() const
    {
        return m_selected_quest_id != -1;
    }
}
