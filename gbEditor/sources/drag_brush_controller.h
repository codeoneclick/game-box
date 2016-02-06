//
//  brush_controller.hpp
//  gbEditor
//
//  Created by Serhii Serhiiv on 2/2/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef drag_brush_controller_h
#define drag_brush_controller_h

#include "drag_controller.h"

namespace gb
{
    namespace ed
    {
        class drag_brush_controller : public drag_controller
        {
        private:
            
        protected:

            game_object_shared_ptr m_grid;
            landscape_shared_ptr m_landscape;
            brush_shared_ptr m_brush;
            
            void on_touched(const ces_entity_shared_ptr&, const glm::vec2& point, e_input_source input_source, e_input_state input_state);
            void on_dragged(const ces_entity_shared_ptr&, const glm::vec2& point, e_input_source input_source, e_input_state input_state);
            void on_moved(const ces_entity_shared_ptr&, const glm::vec2& point, e_input_source input_source, e_input_state input_state);
            
        public:
            
            drag_brush_controller(const landscape_shared_ptr& landscape, const brush_shared_ptr& brush);
            ~drag_brush_controller();
            
            void set_grid(const game_object_shared_ptr& grid);
            
            void set_active_brush(i32 layer, i32 sampler);
        };
    };
};

#endif 
