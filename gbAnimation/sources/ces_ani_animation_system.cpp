//
//  ces_ani_animation_system.cpp
//  gbAnimation
//
//  Created by serhii serhiiv on 11/14/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_ani_animation_system.h"
#include "ces_transformation_component.h"
#include "ces_ani_main_timeline_component.h"
#include "ces_ani_frame_component.h"
#include "ani_timeline.h"
#include "ani_animation_sequence.h"
#include "ani_animation_frame.h"
#include "ani_subobject_state.h"
#include "ces_transformation_extension.h"

namespace gb
{
    namespace anim
    {
        ces_ani_animation_system_weak_ptr ces_ani_animation_system::g_shared_instance;
        
        ces_ani_animation_system::ces_ani_animation_system()
        {

        }
        
        ces_ani_animation_system::~ces_ani_animation_system()
        {

        }
        
        void ces_ani_animation_system::init()
        {
            g_shared_instance = std::static_pointer_cast<ces_ani_animation_system>(shared_from_this());
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
                                                                     ani_display_list_container_const_shared_ptr display_list)
        {
            auto timeline_component = entity->get_component<ces_ani_base_timeline_component>();
            if(timeline_component)
            {
                i32 object_id_reference_to_insert = -1;
                i32 current_object_id_reference = timeline_component->object_id_reference;
                for (const auto& state : display_list->m_states)
                {
                    if(state->m_object_id_reference == current_object_id_reference && state->get_visible())
                    {
                        object_id_reference_to_insert = current_object_id_reference;
                        break;
                    }
                }
                if(object_id_reference_to_insert != -1)
                {
                    assert(display_list->m_entities.find(object_id_reference_to_insert) == display_list->m_entities.end());
                    display_list->m_entities.insert(std::make_pair(object_id_reference_to_insert, entity));
                    
                    std::shared_ptr<ani_timeline> timeline = timeline_component->timeline;
                    const ani_animation_frames_t& animation_frames = timeline->get_animation_frames();
                    i32 current_frame_index = timeline_component->current_frame_index;
                    if (animation_frames.size() > current_frame_index)
                    {
                        std::shared_ptr<ani_animation_frame> current_frame = animation_frames[current_frame_index];
                        const ani_animation_frame::ani_subobject_states_t& current_states = current_frame->get_object_states();
                        
                        auto current_display_list = std::make_shared<ani_display_list_container>();
                        current_display_list->m_states.insert(current_display_list->m_states.end(), current_states.begin(), current_states.end());
                        display_list->m_container.push_back(current_display_list);
                        
                        std::list<ces_entity_shared_ptr> children = entity->children;
                        for(const auto& child : children)
                        {
                            ces_ani_animation_system::fill_display_list_recursively(child, current_display_list);
                        }
                    }
                }
                else
                {
                    entity->visible = false;
                    return;
                }
            }
            
            auto frame_component = entity->get_component<ces_ani_frame_component>();
            if(frame_component)
            {
                i32 object_id_reference_to_insert = -1;
                i32 current_object_id_reference = frame_component->object_id_reference;
                for (const auto& state : display_list->m_states)
                {
                    if(state->m_object_id_reference == current_object_id_reference && state->get_visible())
                    {
                        object_id_reference_to_insert = current_object_id_reference;
                        break;
                    }
                }
                if(object_id_reference_to_insert != -1)
                {
                    assert(display_list->m_entities.find(object_id_reference_to_insert) == display_list->m_entities.end());
                    display_list->m_entities.insert(std::make_pair(object_id_reference_to_insert, entity));
                }
                else
                {
                    entity->visible = false;
                    return;
                }
            }
            
            if(!timeline_component && !frame_component)
            {
                std::list<ces_entity_shared_ptr> children = entity->children;
                for(const auto& child : children)
                {
                    ces_ani_animation_system::fill_display_list_recursively(child, display_list);
                }
            }
        }
        
        void ces_ani_animation_system::update_recursively(const ces_entity_shared_ptr& entity, f32 dt)
        {
            auto main_timeline_component = entity->get_component<ces_ani_main_timeline_component>();
            if(main_timeline_component)
            {
                auto current_display_list = std::make_shared<ani_display_list_container>();
                
                
                std::shared_ptr<ani_timeline> main_timeline = main_timeline_component->timeline;
                const ani_animation_frames_t& animation_frames = main_timeline->get_animation_frames();
                i32 current_frame_index = main_timeline_component->current_frame_index;
                if (animation_frames.size() > current_frame_index)
                {
                    std::shared_ptr<ani_animation_frame> current_frame = animation_frames[current_frame_index];
                    const ani_animation_frame::ani_subobject_states_t& current_states = current_frame->get_object_states();
                    current_display_list->m_states.insert(current_display_list->m_states.end(), current_states.begin(), current_states.end());
                    
                    std::list<ces_entity_shared_ptr> children = entity->children;
                    for(const auto& child : children)
                    {
                        ces_ani_animation_system::fill_display_list_recursively(child, current_display_list);
                    }
                    ces_ani_animation_system::update_display_list_recursively(current_display_list, dt);
                }
                
                ces_transformation_extension::update_absolute_transformation_recursively(entity);
                main_timeline_component->next_frame(dt);
            }
            
            std::list<ces_entity_shared_ptr> children = entity->children;
            for(const auto& child : children)
            {
                ces_ani_animation_system::update_recursively(child, dt);
            }
        }
        
        void ces_ani_animation_system::update_display_list_recursively(ani_display_list_container_const_shared_ptr display_list, f32 dt)
        {
            for (const auto& state : display_list->m_states)
            {
                const auto& iterator = display_list->m_entities.find(state->m_object_id_reference);
                
                if(iterator != display_list->m_entities.end())
                {
                    auto base_timeline_component = iterator->second->get_component<ces_ani_base_timeline_component>();
                    auto frame_component = iterator->second->get_component<ces_ani_frame_component>();
                    if(frame_component)
                    {
                        ces_transformation_component_shared_ptr transformation_component = iterator->second->get_component<ces_transformation_component>();
                        
                        f32 rotation = state->get_rotation();
                        if(frame_component->is_cw90)
                        {
                            rotation += 90.f;
                        }
                        
                        transformation_component->set_scale(state->get_scale());
                        transformation_component->set_position(state->get_position());
                        transformation_component->set_rotation(rotation);
                        iterator->second->visible = true;
                    }
                    else if(base_timeline_component)
                    {
                        ces_transformation_component_shared_ptr transformation_component = iterator->second->get_component<ces_transformation_component>();
                        transformation_component->set_scale(state->get_scale());
                        transformation_component->set_position(state->get_position());
                        transformation_component->set_rotation(state->get_rotation());
                        iterator->second->visible = true;
                        
                        base_timeline_component->next_frame(dt);
                    }
                }
            }
            
            for(const auto& child_display_list : display_list->m_container)
            {
                ces_ani_animation_system::update_display_list_recursively(child_display_list, dt);
            }
        }
        
        void ces_ani_animation_system::setup_animation(ces_entity_const_shared_ptr entity, i32 frame_index, bool is_playing, bool is_looped)
        {
            auto main_timeline_animation_component = entity->get_component<ces_ani_main_timeline_component>();
            if(main_timeline_animation_component)
            {
                ani_timeline_shared_ptr main_timeline = main_timeline_animation_component->timeline;
                const ani_animation_frames_t& animation_frames = main_timeline->get_animation_frames();
                if (animation_frames.size() > frame_index)
                {
                    main_timeline_animation_component->current_frame_index = frame_index;
                    main_timeline_animation_component->is_playing = is_playing;
                    main_timeline_animation_component->is_looped = is_looped;
                    if(!g_shared_instance.expired())
                    {
                        ces_ani_animation_system_shared_ptr shared_instacnce = g_shared_instance.lock();
                        shared_instacnce->update_recursively(entity, 0);
                    }
                }
                else
                {
                    assert(false);
                }
            }
            else
            {
                assert(false);
            }
        }
        
        void ces_ani_animation_system::goto_and_stop(ces_entity_const_shared_ptr entity, i32 frame_index)
        {
            ces_ani_animation_system::setup_animation(entity, frame_index);
        }
        
        void ces_ani_animation_system::goto_and_play(ces_entity_const_shared_ptr entity, i32 frame_index, bool is_looped)
        {
            ces_ani_animation_system::setup_animation(entity, frame_index, true, is_looped);
        }
        
        void ces_ani_animation_system::goto_and_stop(ces_entity_const_shared_ptr entity, const std::string& animation_name)
        {
            auto timeline_animation_component = entity->get_component<ces_ani_main_timeline_component>();
            if(timeline_animation_component)
            {
                ani_timeline_shared_ptr timeline = timeline_animation_component->timeline;
                ani_animation_sequence_shared_ptr sequence = timeline->get_sequence(animation_name);
                if(sequence)
                {
                    ces_ani_animation_system::setup_animation(entity, sequence->m_start_frame);
                }
                else
                {
                    assert(false);
                }
            }
        }
        
        void ces_ani_animation_system::goto_and_play(ces_entity_const_shared_ptr entity, const std::string& animation_name, bool is_looped)
        {
            auto timeline_animation_component = entity->get_component<ces_ani_main_timeline_component>();
            if(timeline_animation_component)
            {
                ani_timeline_shared_ptr timeline = timeline_animation_component->timeline;
                ani_animation_sequence_shared_ptr sequence = timeline->get_sequence(animation_name);
                if(sequence)
                {
                    ces_ani_animation_system::setup_animation(entity, sequence->m_start_frame, true, is_looped);
                }
                else
                {
                    assert(false);
                }
            }
        }
    }
}
