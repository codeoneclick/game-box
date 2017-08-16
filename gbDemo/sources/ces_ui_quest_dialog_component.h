//
//  ces_ui_quest_dialog_component.h
//  gbDemo
//
//  Created by serhii serhiiv on 8/8/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"
#include "table_view_cell.h"

namespace game
{
    class ces_ui_quest_dialog_component : public gb::ces_base_component
    {
    public:
        
        class quest_table_view_cell_data : public gb::ui::table_view_cell_data
        {
        private:
            
        protected:
            
            i32 m_quest_id;
            
        public:
            
            quest_table_view_cell_data();
            ~quest_table_view_cell_data();
            
            void set_quest_id(i32 id);
            i32 get_quest_id() const;
        };
        
        class quest_table_view_cell : public gb::ui::table_view_cell
        {
        public:
            
            typedef std::function<void(i32)> accept_quest_button_callback_t;
            
            static const std::string k_accept_quest_button;
            static const std::string k_in_progress_quest_button;
            
        private:
            
        protected:
            
            accept_quest_button_callback_t m_accept_quest_button_callback;
            
        public:
            
            quest_table_view_cell(const gb::scene_fabricator_shared_ptr& fabricator, i32 index, const std::string& identifier);
            ~quest_table_view_cell();
            
            void setup_components() override;
            void create() override;
            
            void set_accept_quest_button_callback(const accept_quest_button_callback_t& callback);
            void set_quest_in_progress(bool value);
        };
        
    private:
        
        i32 m_selected_quest_id;
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_ui_quest_dialog_component, gb::ces_base_component::g_guids_container)
        ces_ui_quest_dialog_component();
        ~ces_ui_quest_dialog_component() = default;
        
        void set_selected_quest_id(i32 id);
        i32 get_selected_quest_id() const;
        
        bool is_selected_quest_id_exist() const;
    };
};
