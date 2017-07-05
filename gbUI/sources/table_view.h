//
//  table_view.h
//  gbUI
//
//  Created by sergey.sergeev on 1/15/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef table_view_h
#define table_view_h

#include "control.h"
#include "input_context.h"

namespace gb
{
    namespace ui
    {
        class table_view : public control
        {
        public:
            
            typedef std::function<table_view_cell_shared_ptr(i32, const table_view_cell_data_shared_ptr&, const ces_entity_shared_ptr&)> t_get_cell_callback;
            typedef std::function<f32(i32)> t_get_cell_height_callback;
            
        private:
            
        protected:
            
            t_get_cell_callback m_get_cell_callback;
            t_get_cell_height_callback m_get_cell_height_callback;
            
            glm::vec2 m_separator_offset;
            glm::vec2 m_previous_dragged_point;
            f32 m_drag_events_sum;
            i32 m_drag_events_count;
            f32 m_scroll_inertia;
            
            std::list<table_view_cell_shared_ptr> m_unused_cells;
            std::list<table_view_cell_shared_ptr> m_cells;
            
            std::vector<table_view_cell_data_shared_ptr> m_data_source;
            
            void scroll_content(f32 delta);
            
            void on_touched(const ces_entity_shared_ptr&,
                            const glm::vec2& touch_point,
                            e_input_source input_source,
                            e_input_state input_state);
            void on_autoscroll(const gb::ces_entity_shared_ptr& entity, f32 deltatime);
            
            void fill_cell(i32 index, i32 direction);
            
            void clip_invisible_cells(i32 direction);
            void add_visible_cells(i32 direction);
            
        public:
            
            table_view(const scene_fabricator_shared_ptr& fabricator);
            ~table_view();
            
            static table_view_shared_ptr construct(const scene_fabricator_shared_ptr& fabricator);
            
            void create();
            
            void set_separator_offset(const glm::vec2& separator_offset);
            
            void set_on_get_cell_callback(const t_get_cell_callback& callback);
            void set_on_get_table_cell_height_callback(const t_get_cell_height_callback& callback);
            
            void set_data_source(const std::vector<table_view_cell_data_shared_ptr>& data_source);
            void reload_data();
            
            table_view_cell_shared_ptr reuse_cell(const std::string& identifier, i32 index);
        };
    };
};

#endif
