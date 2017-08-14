//
//  table_view_cell.h
//  gbUI
//
//  Created by sergey.sergeev on 1/22/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "control.h"
#include "input_context.h"

namespace gb
{
    namespace ui
    {
        class table_view_cell_data
        {
        private:
            
        protected:
            
        public:
            
            table_view_cell_data() = default;
            virtual ~table_view_cell_data() = default;
        };

        class table_view_cell : public control
        {
        private:
            
            friend class table_view;
            
        protected:
            
            i32 m_index;
            std::string m_identifier;
            
            void on_touched(const ces_entity_shared_ptr&, const glm::vec2& point,
                            e_input_source input_source, e_input_state input_state);
            
            void set_index(i32 index);
            
        public:
            
            table_view_cell(const scene_fabricator_shared_ptr& fabricator, i32 index, const std::string& identifier);
            ~table_view_cell();
            
            void setup_components() override;
            void create() override;
            
            i32 get_index() const;
            std::string get_identifier() const;
        };
    };
};
