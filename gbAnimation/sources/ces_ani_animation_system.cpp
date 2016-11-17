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
#include "ces_ani_main_timeline_animation_component.h"
#include "ces_ani_timeline_animation_component.h"
#include "ces_ani_image_animation_component.h"
#include "ani_timeline.h"
#include "ani_animation_frame.h"
#include "ani_subobject_state.h"
#include "ces_transformation_extension.h"

namespace gb
{
    namespace anim
    {
        ces_ani_animation_system* ces_ani_animation_system::g_shared_instance = nullptr;
        
        ces_ani_animation_system::ces_ani_animation_system()
        {
            g_shared_instance = this;
        }
        
        ces_ani_animation_system::~ces_ani_animation_system()
        {
            g_shared_instance = nullptr;
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
        
        void ces_ani_animation_system::fill_display_list_recursively(const ces_entity_shared_ptr& entity,
                                                                    std::unordered_map<i32 ,ces_entity_shared_ptr>& display_list,
                                                                     const ani_animation_frame::ani_subobject_states_t& states)
        {
            std::shared_ptr<ces_ani_timeline_animation_component> timeline_animation_component = entity->get_component<ces_ani_timeline_animation_component>();
            if(timeline_animation_component)
            {
                i32 object_id_reference_to_insert = -1;
                i32 current_object_id_reference = timeline_animation_component->object_id_reference;
                for (const auto& state : states)
                {
                    if(state->m_object_id_reference == current_object_id_reference && state->get_visible())
                    {
                        object_id_reference_to_insert = current_object_id_reference;
                        break;
                    }
                }
                if(object_id_reference_to_insert != -1)
                {
                    display_list[object_id_reference_to_insert] = entity;
                    
                    std::shared_ptr<ani_timeline> timeline = timeline_animation_component->timeline;
                    const ani_animation_frames_t& animation_frames = timeline->get_animation_frames();
                    i32 current_frame_index = timeline_animation_component->current_frame_index;
                    if (animation_frames.size() > current_frame_index)
                    {
                        std::shared_ptr<ani_animation_frame> current_frame = animation_frames[current_frame_index];
                        const ani_animation_frame::ani_subobject_states_t& states = current_frame->get_object_states();
                        
                        std::list<ces_entity_shared_ptr> children = entity->children;
                        for(const auto& child : children)
                        {
                            ces_ani_animation_system::fill_display_list_recursively(child, display_list, states);
                        }
                    }
                }
                else
                {
                    entity->visible = false;
                    return;
                }
            }
            
            std::shared_ptr<ces_ani_image_animation_component> image_animation_component = entity->get_component<ces_ani_image_animation_component>();
            if(image_animation_component)
            {
                i32 object_id_reference_to_insert = -1;
                i32 current_object_id_reference = image_animation_component->object_id_reference;
                for (const auto& state : states)
                {
                    if(state->m_object_id_reference == current_object_id_reference && state->get_visible())
                    {
                        object_id_reference_to_insert = current_object_id_reference;
                        break;
                    }
                }
                if(object_id_reference_to_insert != -1)
                {
                    display_list[object_id_reference_to_insert] = entity;
                }
                else
                {
                    entity->visible = false;
                    return;
                }
            }
            
            if(!timeline_animation_component && !image_animation_component)
            {
                std::list<ces_entity_shared_ptr> children = entity->children;
                for(const auto& child : children)
                {
                    ces_ani_animation_system::fill_display_list_recursively(child, display_list, states);
                }
            }
        }
        
        void ces_ani_animation_system::update_recursively(const ces_entity_shared_ptr& entity, f32 deltatime)
        {
            std::shared_ptr<ces_ani_animation_component> animation_component = entity->get_component<ces_ani_animation_component>();
            ces_transformation_component_shared_ptr transformation_component = entity->get_component<ces_transformation_component>();
            
            std::shared_ptr<ces_ani_main_timeline_animation_component> main_timeline_animation_component = entity->get_component<ces_ani_main_timeline_animation_component>();
            if(main_timeline_animation_component)
            {
                std::unordered_map<i32 ,ces_entity_shared_ptr> display_list;
                
                std::shared_ptr<ani_timeline> main_timeline = main_timeline_animation_component->timeline;
                const ani_animation_frames_t& animation_frames = main_timeline->get_animation_frames();
                i32 current_frame_index = main_timeline_animation_component->current_frame_index;
                if (animation_frames.size() > current_frame_index)
                {
                    std::shared_ptr<ani_animation_frame> current_frame = animation_frames[current_frame_index];
                    const ani_animation_frame::ani_subobject_states_t& main_states = current_frame->get_object_states();
                    
                    std::list<ces_entity_shared_ptr> children = entity->children;
                    for(const auto& child : children)
                    {
                        ces_ani_animation_system::fill_display_list_recursively(child, display_list, main_states);
                    }
                    
                    for (const auto& state : main_states)
                    {
                        const auto& iterator = display_list.find(state->m_object_id_reference);
                        if(iterator != display_list.end())
                        {
                            std::shared_ptr<ces_ani_timeline_animation_component> timeline_animation_component = iterator->second->get_component<ces_ani_timeline_animation_component>();
                            std::shared_ptr<ces_ani_image_animation_component> image_animation_component = iterator->second->get_component<ces_ani_image_animation_component>();
                            if(image_animation_component)
                            {
                                ces_transformation_component_shared_ptr transformation_component = iterator->second->get_component<ces_transformation_component>();
                                
                                f32 rotation = state->get_rotation();
                                if(image_animation_component->is_cw90)
                                {
                                    rotation += 90.f;
                                }
                                
                                transformation_component->set_scale(state->get_scale());
                                transformation_component->set_position(state->get_position());
                                transformation_component->set_rotation(rotation);
                                iterator->second->visible = true;
                            }
                            else if(timeline_animation_component)
                            {
                                ces_transformation_component_shared_ptr transformation_component = iterator->second->get_component<ces_transformation_component>();
                                transformation_component->set_scale(state->get_scale());
                                transformation_component->set_position(state->get_position());
                                transformation_component->set_rotation(state->get_rotation());
                                iterator->second->visible = true;
                                
                                timeline_animation_component->goto_next_frame();
                            }
                        }
                    }
                }
                
                ces_transformation_extension::update_absolute_transformation_recursively(entity);
                main_timeline_animation_component->goto_next_frame();
            }
            
            std::list<ces_entity_shared_ptr> children = entity->children;
            for(const auto& child : children)
            {
                ces_ani_animation_system::update_recursively(child, deltatime);
            }
        }
        
        void ces_ani_animation_system::goto_and_stop(const ces_entity_shared_ptr& entity, i32 frame_index)
        {
            g_shared_instance->update_recursively(entity, 0);
        }
    }
}
