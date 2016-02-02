//
//  game_objects_drag_controller.h
//  gbDemo
//
//  Created by sergey.sergeev on 1/21/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef game_objects_drag_controller_h
#define game_objects_drag_controller_h

#include "ces_entity.h"
#include "input_context.h"
#include "ed_declarations.h"

namespace cs
{
    class game_objects_drag_controller
    {
    private:
        
    protected:
        
        glm::vec2 m_previous_dragged_point;
        
        gb::ed::stroke_shared_ptr m_stroke_object;
        
        std::set<gb::game_object_shared_ptr> m_game_objects;
        gb::game_object_shared_ptr m_selected_game_object;
        
        void on_touched(const gb::ces_entity_shared_ptr&, const glm::vec2& point, gb::e_input_source input_source, gb::e_input_state input_state);
        void on_dragged(const gb::ces_entity_shared_ptr&, const glm::vec2& point, gb::e_input_source input_source, gb::e_input_state input_state);
        
    public:
        
        game_objects_drag_controller(const gb::ed::stroke_shared_ptr& stroke_object);
        ~game_objects_drag_controller();
        
        void add_game_object(const gb::game_object_shared_ptr& game_object);
        void remove_game_object(const gb::game_object_shared_ptr& game_object);
    };
};

#endif
