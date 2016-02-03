//
//  drag_camera_controller.h
//  gbDemo
//
//  Created by sergey.sergeev on 1/15/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef drag_camera_controller_h
#define drag_camera_controller_h

#include "drag_controller.h"

namespace gb
{
    namespace ed
    {
        class drag_camera_controller : public drag_controller
        {
        private:
            
        protected:
            
            camera_shared_ptr m_camera;
            game_object_shared_ptr m_grid;
            
            void on_touched(const ces_entity_shared_ptr&, const glm::vec2& point, e_input_source input_source, e_input_state input_state);
            void on_dragged(const ces_entity_shared_ptr&, const glm::vec2& point, e_input_source input_source, e_input_state input_state);
            
        public:
            
            drag_camera_controller(const camera_shared_ptr& camera);
            ~drag_camera_controller();
            
            void set_grid(const game_object_shared_ptr& grid);
        };
    };
};

#endif
