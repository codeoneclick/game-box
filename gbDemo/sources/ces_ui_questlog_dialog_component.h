//
//  ces_ui_questlog_dialog_component.h
//  gbDemo
//
//  Created by serhii serhiiv on 8/14/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"
#include "table_view_cell.h"

namespace game
{
    class ces_ui_questlog_dialog_component : public gb::ces_base_component
    {
    public:
        
        class quest_table_view_cell_data : public gb::ui::table_view_cell_data
        {
        private:
            
        protected:
            
        public:
            
            quest_table_view_cell_data();
            ~quest_table_view_cell_data();
        };
        
        class quest_table_view_cell : public gb::ui::table_view_cell
        {
        public:
            
            static const std::string k_track_quest_button;
            static const std::string k_remove_quest_button;
        
        private:
            
        protected:
            
        public:
            
            quest_table_view_cell(const gb::scene_fabricator_shared_ptr& fabricator, i32 index, const std::string& identifier);
            ~quest_table_view_cell();
            
            void setup_components() override;
            void create() override;
        };
    
    private:
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_ui_questlog_dialog_component, gb::ces_base_component::g_guids_container)
        ces_ui_questlog_dialog_component();
        ~ces_ui_questlog_dialog_component() = default;
    };
};

