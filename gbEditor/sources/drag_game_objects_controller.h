//
//  game_objects_drag_controller.h
//  gbDemo
//
//  Created by sergey.sergeev on 1/21/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef drag_game_objects_controller_h
#define drag_game_objects_controller_h

#include "drag_controller.h"

namespace gb
{
    namespace ed
    {
        class drag_game_objects_controller : public drag_controller
        {
        private:
            
        protected:
            
            stroke_shared_ptr m_stroke_object;
            
            std::set<game_object_shared_ptr> m_game_objects;
            game_object_shared_ptr m_selected_game_object;
            
            void on_touched(const ces_entity_shared_ptr&, const glm::vec2& point,
                            e_input_source input_source, e_input_state input_state);
            void on_dragged(const ces_entity_shared_ptr&, const glm::vec2& point,
                            e_input_source input_source, e_input_state input_state);
            
        public:
            
            drag_game_objects_controller(const gb::ed::stroke_shared_ptr& stroke_object);
            ~drag_game_objects_controller();
            
            void add_game_object(const gb::game_object_shared_ptr& game_object);
            void remove_game_object(const gb::game_object_shared_ptr& game_object);
        };
    };
};

#endif
