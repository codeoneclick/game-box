//
//  ces_ani_animation_system.cpp
//  gbAnimation
//
//  Created by serhii serhiiv on 11/14/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_ani_animation_system.h"
#include "ces_ani_animation_component.h"
#include "ces_transformation_component.h"
#include "ani_timeline.h"
#include "ani_animation_frame.h"
#include "ani_subobject_state.h"
#include "ces_transformation_extension.h"

namespace gb
{
    namespace anim
    {
        ces_ani_animation_system::ces_ani_animation_system()
        {
            
        }
        
        ces_ani_animation_system::~ces_ani_animation_system()
        {
            
        }
        
        void ces_ani_animation_system::on_feed_start(f32 deltatime)
        {
            
        }
        
        void ces_ani_animation_system::on_feed(const ces_entity_shared_ptr& entity, f32 deltatime)
        {
            ces_ani_animation_system::update_recursively(entity, deltatime);
        }
        
        void ces_ani_animation_system::on_feed_end(f32 deltatime)
        {
            
        }
        
        void ces_ani_animation_system::update_recursively(const ces_entity_shared_ptr& entity, f32 deltatime)
        {
            std::shared_ptr<ces_ani_animation_component> animation_component = entity->get_component<ces_ani_animation_component>();
            ces_transformation_component_shared_ptr transformation_component = entity->get_component<ces_transformation_component>();
            
            if(animation_component)
            {
                std::shared_ptr<ani_timeline> timeline = animation_component->timeline;
                const ani_animation_frames_t& animation_frames = timeline->get_animation_frames();
                i32 current_frame_index = animation_component->current_frame_index;
                if (animation_frames.size() > current_frame_index)
                {
                    std::shared_ptr<ani_animation_frame> current_frame = animation_frames[current_frame_index];
                    const ani_animation_frame::ani_subobject_states_t& states = current_frame->get_object_states();
                    i32 current_object_id_reference = animation_component->object_id_reference;
                    for (const auto& state : states)
                    {
                        if (state->m_object_id_reference != current_object_id_reference)
                        {
                            continue;
                        }
                        f32 rotation = state->get_rotation();
                        if(animation_component->is_cw90)
                        {
                            rotation += 90.f;
                        }
                        
                        transformation_component->set_scale(state->get_scale());
                        transformation_component->set_position(state->get_position());
                        transformation_component->set_rotation(rotation);
                    }
                }
                ces_transformation_extension::update_absolute_transformation_recursively(entity);
                animation_component->goto_next_frame();
            }
            
            std::list<ces_entity_shared_ptr> children = entity->children;
            for(const auto& child : children)
            {
                ces_ani_animation_system::update_recursively(child, deltatime);
            }
        }
    }
}
