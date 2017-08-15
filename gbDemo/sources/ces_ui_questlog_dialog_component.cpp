//
//  ces_ui_questlog_dialog_component.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 8/14/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_ui_questlog_dialog_component.h"
#include "button.h"
#include "ces_bound_touch_component.h"

namespace game
{
    ces_ui_questlog_dialog_component::quest_table_view_cell_data::quest_table_view_cell_data()
    {
        
    }
    
    ces_ui_questlog_dialog_component::quest_table_view_cell_data::~quest_table_view_cell_data()
    {
        
    }
    
    
    const std::string ces_ui_questlog_dialog_component::quest_table_view_cell::k_track_quest_button = "track_quest_button";
    const std::string ces_ui_questlog_dialog_component::quest_table_view_cell::k_remove_quest_button = "remove_quest_button";
    
    ces_ui_questlog_dialog_component::quest_table_view_cell::quest_table_view_cell(const gb::scene_fabricator_shared_ptr& fabricator, i32 index, const std::string& identifier) :
    gb::ui::table_view_cell(fabricator, index, identifier)
    {
        size.setter([=](const glm::vec2& size) {
            
            m_size = size;
            auto bound_touch_component = ces_entity::get_component<gb::ces_bound_touch_component>();
            bound_touch_component->set_bounds(glm::vec4(0.f, 0.f, m_size.x, m_size.y));
            m_elements[control::k_background_element_name]->size = size;
            
            m_elements[k_track_quest_button]->position = glm::vec2(m_size.x - 128.f - 8.f, 8.f);
            m_elements[k_remove_quest_button]->position = glm::vec2(m_size.x - 128.f - 8.f, 8.f + 24.f + 16.f);
        });
    }
    
    ces_ui_questlog_dialog_component::quest_table_view_cell::~quest_table_view_cell()
    {
        
    }
    
    void ces_ui_questlog_dialog_component::quest_table_view_cell::setup_components()
    {
        gb::ui::table_view_cell::setup_components();
    }
    
    void ces_ui_questlog_dialog_component::quest_table_view_cell::create()
    {
        gb::ui::table_view_cell::create();
        
        glm::vec2 cell_size = quest_table_view_cell::size;
        
        auto track_quest_button = gb::ces_entity::construct<gb::ui::button>(control::get_fabricator());
        track_quest_button->create();
        track_quest_button->size = glm::vec2(128.f, 24.f);
        track_quest_button->position = glm::vec2(cell_size.x - 128.f - 8.f, 8.f);
        track_quest_button->set_text("Track");
        m_elements[k_track_quest_button] = track_quest_button;
        quest_table_view_cell::add_child(track_quest_button);
        
        auto remove_quest_button = gb::ces_entity::construct<gb::ui::button>(control::get_fabricator());
        remove_quest_button->create();
        remove_quest_button->size = glm::vec2(128.f, 24.f);
        remove_quest_button->position = glm::vec2(cell_size.x - 128.f - 8.f, 8.f + 24.f + 16.f);
        remove_quest_button->set_text("Remove");
        m_elements[k_remove_quest_button] = remove_quest_button;
        quest_table_view_cell::add_child(remove_quest_button);
    }
    
    ces_ui_questlog_dialog_component::ces_ui_questlog_dialog_component()
    {
        
    }
}
