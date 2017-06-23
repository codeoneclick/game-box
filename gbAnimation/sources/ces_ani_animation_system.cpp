//
//  ces_ani_animation_system.cpp
//  gbAnimation
//
//  Created by serhii serhiiv on 11/14/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_ani_animation_system.h"
#include "ces_transformation_2d_component.h"
#include "ces_ani_timeline_component.h"
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
        
        ces_ani_animation_system::ces_ani_animation_system() :
        m_global_frame_iteration(0)
        {

        }
        
        ces_ani_animation_system::~ces_ani_animation_system()
        {

        }
        
        void ces_ani_animation_system::init()
        {
            g_shared_instance = std::static_pointer_cast<ces_ani_animation_system>(shared_from_this());
        }
        
        void ces_ani_animation_system::update_global_frame()
        {
            m_global_frame_iteration++;
        }
        
        void ces_ani_animation_system::on_feed_start(f32 deltatime)
        {
            ces_ani_animation_system::update_global_frame();
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
            auto timeline_component = entity->get_component<ces_ani_timeline_component>();
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
                        
                        std::vector<ces_entity_shared_ptr> children = entity->children;
                        for(const auto& child : children)
                        {
                            ces_ani_animation_system::fill_display_list_recursively(child, current_display_list);
                        }
                    }
                }
                else
                {
                    entity->visible_in_next_frame = false;
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
                    entity->visible_in_next_frame = false;
                    return;
                }
            }
            
            if(!timeline_component && !frame_component)
            {
                std::vector<ces_entity_shared_ptr> children = entity->children;
                for(const auto& child : children)
                {
                    ces_ani_animation_system::fill_display_list_recursively(child, display_list);
                }
            }
        }
        
        void ces_ani_animation_system::update_recursively(const ces_entity_shared_ptr& entity, f32 dt, bool force)
        {
            auto timeline_component = entity->get_component<ces_ani_timeline_component>();
            if(timeline_component)
            {
                ui64 current_frame_iteration = timeline_component->frame_iteration;
                bool is_playing = timeline_component->is_playing || force;
                if(current_frame_iteration != m_global_frame_iteration && is_playing)
                {
                    auto current_display_list = std::make_shared<ani_display_list_container>();
                    
                    std::shared_ptr<ani_timeline> main_timeline = timeline_component->timeline;
                    const ani_animation_frames_t& animation_frames = main_timeline->get_animation_frames();
                    i32 current_frame_index = timeline_component->current_frame_index;
                    if (animation_frames.size() > current_frame_index)
                    {
                        std::shared_ptr<ani_animation_frame> current_frame = animation_frames[current_frame_index];
                        const ani_animation_frame::ani_subobject_states_t& current_states = current_frame->get_object_states();
                        current_display_list->m_states.insert(current_display_list->m_states.end(), current_states.begin(), current_states.end());
                        
                        std::vector<ces_entity_shared_ptr> children = entity->children;
                        for(const auto& child : children)
                        {
                            ces_ani_animation_system::fill_display_list_recursively(child, current_display_list);
                        }
                        ces_ani_animation_system::update_display_list_recursively(current_display_list, dt);
                    }
                    
                    ces_transformation_extension::update_absolute_transformation_recursively(entity);
                    timeline_component->next_frame(dt);
                    timeline_component->frame_iteration = m_global_frame_iteration;
                }
            }
            
            std::vector<ces_entity_shared_ptr> children = entity->children;
            for(const auto& child : children)
            {
                ces_ani_animation_system::update_recursively(child, dt, force);
            }
        }
        
        void ces_ani_animation_system::update_display_list_recursively(ani_display_list_container_const_shared_ptr display_list, f32 dt)
        {
            std::sort(display_list->m_states.begin(), display_list->m_states.end(),
                      [](std::shared_ptr<ani_subobject_state> state_01, std::shared_ptr<ani_subobject_state> state_02) {
                return state_01->get_z_index() < state_02->get_z_index();
            });
            
            ces_entity_shared_ptr global_parent = display_list->m_entities.begin()->second->parent;
            assert(global_parent);
            
            std::for_each(display_list->m_entities.begin(), display_list->m_entities.end(), [global_parent](const std::pair<i32, ces_entity_shared_ptr>& iterator) {
                ces_entity_shared_ptr parent = iterator.second->parent;
                if(parent != global_parent)
                {
                    std::cout<<"wrong animation hierarchy"<<std::endl;
                    assert(false);
                }
            });
            f32 z_order = global_parent->get_component<ces_transformation_2d_component>()->get_z_order();
            
            for (const auto& state : display_list->m_states)
            {
                const auto& iterator = display_list->m_entities.find(state->m_object_id_reference);
                
                if(iterator != display_list->m_entities.end())
                {
                    auto timeline_component = iterator->second->get_component<ces_ani_timeline_component>();
                    auto frame_component = iterator->second->get_component<ces_ani_frame_component>();
                    if(frame_component)
                    {
                        auto transformation_component = iterator->second->get_component<ces_transformation_2d_component>();
                        
                        f32 rotation = state->get_rotation();
                        if(frame_component->is_cw90)
                        {
                            rotation += 90.f;
                        }
                        
                        transformation_component->set_scale(state->get_scale());
                        transformation_component->set_position(state->get_position());
                        transformation_component->set_z_order(z_order += ces_transformation_2d_component::k_z_order_step);
                        transformation_component->set_rotation(rotation);
                        iterator->second->visible_in_next_frame = true;
                    }
                    else if(timeline_component)
                    {
                        auto transformation_component = iterator->second->get_component<ces_transformation_2d_component>();
                        transformation_component->set_scale(state->get_scale());
                        transformation_component->set_position(state->get_position());
                        transformation_component->set_rotation(state->get_rotation());
                        transformation_component->set_z_order(z_order += ces_transformation_2d_component::k_z_order_step);
                        iterator->second->visible_in_next_frame = true;
                        
                        timeline_component->next_frame(dt);
                        timeline_component->frame_iteration = m_global_frame_iteration;
                    }
                }
            }
            global_parent->rearrange_children_according_to_z_order();
            
            for(const auto& child_display_list : display_list->m_container)
            {
                ces_ani_animation_system::update_display_list_recursively(child_display_list, dt);
            }
        }
        
        void ces_ani_animation_system::update_animation_manualy(ces_entity_const_shared_ptr entity, i32 frame_index, bool is_playing, bool is_looped)
        {
            auto timeline_component = entity->get_component<ces_ani_timeline_component>();
            if(timeline_component)
            {
                ani_timeline_shared_ptr main_timeline = timeline_component->timeline;
                const ani_animation_frames_t& animation_frames = main_timeline->get_animation_frames();
                if (animation_frames.size() > frame_index)
                {
                    timeline_component->current_frame_index = frame_index;
                    timeline_component->is_playing = is_playing;
                    timeline_component->is_looped = is_looped;
                    
                    if(!g_shared_instance.expired())
                    {
                        ces_ani_animation_system_shared_ptr shared_instacnce = g_shared_instance.lock();
                        shared_instacnce->update_global_frame();
                        shared_instacnce->update_recursively(entity, 0, true);
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
            ces_ani_animation_system::update_animation_manualy(entity, frame_index);
        }
        
        void ces_ani_animation_system::goto_and_play(ces_entity_const_shared_ptr entity, i32 frame_index, bool is_looped)
        {
            ces_ani_animation_system::update_animation_manualy(entity, frame_index, true, is_looped);
        }
        
        void ces_ani_animation_system::goto_and_stop(ces_entity_const_shared_ptr entity, const std::string& animation_name)
        {
            auto timeline_component = entity->get_component<ces_ani_timeline_component>();
            if(timeline_component)
            {
                ani_timeline_shared_ptr timeline = timeline_component->timeline;
                ani_animation_sequence_shared_ptr sequence = timeline->get_sequence(animation_name);
                if(sequence)
                {
                    ces_ani_animation_system::update_animation_manualy(entity, sequence->m_start_frame);
                }
                else
                {
                    assert(false);
                }
            }
        }
        
        void ces_ani_animation_system::goto_and_play(ces_entity_const_shared_ptr entity, const std::string& animation_name, bool is_looped)
        {
            auto timeline_component = entity->get_component<ces_ani_timeline_component>();
            if(timeline_component)
            {
                ani_timeline_shared_ptr timeline = timeline_component->timeline;
                ani_animation_sequence_shared_ptr sequence = timeline->get_sequence(animation_name);
                if(sequence)
                {
                    std::string current_playing_sequence_name = timeline_component->current_playing_sequence_name;
                    if(current_playing_sequence_name != animation_name)
                    {
                        timeline_component->set_sequence_interval(sequence->m_start_frame, sequence->m_end_frame);
                        timeline_component->current_playing_sequence_name = animation_name;
                        ces_ani_animation_system::update_animation_manualy(entity, sequence->m_start_frame, true, is_looped);
                    }
                }
                else
                {
                    assert(false);
                }
            }
        }
    }
}
