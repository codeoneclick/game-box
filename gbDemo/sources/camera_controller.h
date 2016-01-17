//
//  camera_controller.h
//  gbDemo
//
//  Created by sergey.sergeev on 1/15/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef camera_controller_h
#define camera_controller_h

#include "ces_entity.h"
#include "input_context.h"

namespace cs
{
    class camera_controller
    {
    private:
        
    protected:

        glm::vec2 m_previous_dragged_point;
        
        gb::camera_shared_ptr m_camera;
        gb::game_object_shared_ptr m_map;
        
        void on_touched(const gb::ces_entity_shared_ptr&, const glm::vec2& point, gb::e_input_element input_element, gb::e_input_state input_state);
        void on_dragged(const gb::ces_entity_shared_ptr&, const glm::vec2& point, gb::e_input_element input_element, gb::e_input_state input_state);
        
    public:
        
        camera_controller(const gb::camera_shared_ptr& camera);
        ~camera_controller();
        
        void set_map(const gb::game_object_shared_ptr& map);
    };
};

#endif
