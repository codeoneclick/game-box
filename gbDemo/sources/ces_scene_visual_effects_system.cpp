//
//  ces_scene_visual_effects_system.cpp
//  gbDemo
//
//  Created by serhii.s on 5/8/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_scene_visual_effects_system.h"
#include "ces_scene_visual_effects_component.h"
#include "ces_render_technique_uniforms_component.h"
#include "ss_render_technique_custom_uniforms.h"
#include "ces_car_descriptor_component.h"
#include "ces_car_statistic_component.h"
#include "ces_car_drift_state_component.h"
#include "ces_shader_uniforms_component.h"
#include "game_object_3d.h"
#include "camera_3d.h"
#include "game_objects_custom_uniforms.h"

namespace game
{
    ces_scene_visual_effects_system::ces_scene_visual_effects_system()
    {
        ces_base_system::add_required_component_guid(m_scene_visual_effects_components_mask, ces_scene_visual_effects_component::class_guid());
        ces_base_system::add_required_component_guid(m_scene_visual_effects_components_mask, gb::ces_render_technique_uniforms_component::class_guid());
        ces_base_system::add_required_components_mask(m_scene_visual_effects_components_mask);
        
         ces_base_system::add_required_component_guid(m_reflection_effect_components_mask, gb::ces_shader_uniforms_component::class_guid());
        ces_base_system::add_required_components_mask(m_reflection_effect_components_mask);
        
        
        ces_base_system::add_required_component_guid(m_car_components_mask, ces_car_descriptor_component::class_guid());
        ces_base_system::add_required_components_mask(m_car_components_mask);
    }
    
    void ces_scene_visual_effects_system::on_feed_start(f32 dt)
    {
        
    }
    
    void ces_scene_visual_effects_system::on_feed(const gb::ces_entity_shared_ptr& root, f32 dt)
    {
        const auto camera_3d = ces_base_system::get_current_camera_3d();
        ces_base_system::enumerate_entities_with_components(m_reflection_effect_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            
            const auto shader_uniforms_component = entity->get_component<gb::ces_shader_uniforms_component>();
            const auto uniforms = shader_uniforms_component->get_uniforms();
            const auto uniforms_set = uniforms->get_uniforms();
            const auto camera_position_uniform_it = uniforms_set.find(sky_reflection_shader_uniforms::k_camera_position_uniform);
            if (camera_position_uniform_it != uniforms_set.end())
            {
                  uniforms->set(glm::vec4(camera_3d->get_position(), 1.f), sky_reflection_shader_uniforms::k_camera_position_uniform);
            }
            const auto i_view_mat_uniform_it = uniforms_set.find(sky_reflection_shader_uniforms::k_i_view_mat_uniform);
            if (i_view_mat_uniform_it != uniforms_set.end())
            {
               uniforms->set(glm::inverse(camera_3d->get_mat_v()), sky_reflection_shader_uniforms::k_i_view_mat_uniform);
            }
        });
        
        ces_base_system::enumerate_entities_with_components(m_car_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            auto car_statistic_component = entity->get_component<ces_car_statistic_component>();
            if(car_statistic_component->mode == ces_car_statistic_component::e_mode::e_mode_player)
            {
                m_main_car = entity;
            }
        });
        
        ces_base_system::enumerate_entities_with_components(m_scene_visual_effects_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            const auto render_technique_uniforms_component = entity->get_component<gb::ces_render_technique_uniforms_component>();
            const auto scene_visual_effects_component = entity->get_component<ces_scene_visual_effects_component>();
            
            bool is_noises_effect_enabled = scene_visual_effects_component->is_noises_enabled;
            bool is_crossfade_effect_enabled = scene_visual_effects_component->is_crossfade_enabled;
            f32 crossfade_effect_progress = scene_visual_effects_component->crossfade_progress;
            
            auto uniforms_wrapper = render_technique_uniforms_component->get_uniforms("ss.tv");
            uniforms_wrapper->set(is_noises_effect_enabled ? 1.f : -1.f, "enabled");
            uniforms_wrapper->set(static_cast<f32>(std::get_tick_count()), "time");
            
            uniforms_wrapper = render_technique_uniforms_component->get_uniforms("ss.crossfade");
            uniforms_wrapper->set(is_crossfade_effect_enabled ? 1.f : -1.f, "enabled");
            uniforms_wrapper->set(crossfade_effect_progress, "progress");
            
            if (!m_main_car.expired())
            {
                const auto car = std::static_pointer_cast<gb::game_object_3d>(m_main_car.lock());
                const auto car_drift_state_component = car->get_component<ces_car_drift_state_component>();
                const auto car_descriptor_component = car->get_component<ces_car_descriptor_component>();
                
                f32 max_collision_protection_time = car_drift_state_component->max_collision_protection_time;
                f32 last_collided_timestamp = car_drift_state_component->last_collided_timestamp;
                f32 current_timestamp = std::get_tick_count();
                f32 collision_power = 0.f;
                bool is_collided = false;
                if (current_timestamp - last_collided_timestamp < max_collision_protection_time * .5f)
                {
                    is_collided = true;
                    collision_power = (current_timestamp - last_collided_timestamp) / (max_collision_protection_time * .5);
                }
                
                f32 slow_motion_power = car_descriptor_component->slow_motion_power;
                bool is_in_slow_motion = slow_motion_power > 0.f;
                bool should_show_collision_vignetting = is_collided;
                bool should_show_slow_motion_vignetting = is_in_slow_motion;
                
                f32 motion_blur_effect_power = 0.f;
                if (is_in_slow_motion)
                {
                    motion_blur_effect_power = car_descriptor_component->motion_blur_effect_power;
                    motion_blur_effect_power += dt;
                    motion_blur_effect_power = glm::clamp(motion_blur_effect_power, 0.f, 1.f);
                    car_descriptor_component->motion_blur_effect_power = motion_blur_effect_power;
                }
                else
                {
                    motion_blur_effect_power = car_descriptor_component->motion_blur_effect_power;
                    motion_blur_effect_power -= dt;
                    motion_blur_effect_power = glm::clamp(motion_blur_effect_power, 0.f, 1.f);
                    car_descriptor_component->motion_blur_effect_power = motion_blur_effect_power;
                }
                
                f32 angular_velocity = car_descriptor_component->angular_velocity;
                angular_velocity = glm::clamp(angular_velocity / static_cast<f32>(M_PI), -1.f, 1.f);
                glm::vec2 motion_direction = glm::vec2(angular_velocity, 1.f);
                
                uniforms_wrapper = render_technique_uniforms_component->get_uniforms_as<ss_output_shader_uniforms>("ss.compose");
                uniforms_wrapper->set(glm::mix(glm::vec4(motion_direction.x, motion_direction.y, 0.f, 0.f),
                                               glm::vec4(motion_direction.x, motion_direction.y, motion_blur_effect_power, 0.f),
                                               motion_blur_effect_power), "motion_direction");
                
                if (should_show_slow_motion_vignetting && !should_show_collision_vignetting)
                {
                    uniforms_wrapper = render_technique_uniforms_component->get_uniforms("ss.compose");
                    uniforms_wrapper->set(glm::vec4(.5f, 1.f, 1.f, 1.f), "vignetting_color");
                    const auto vignetting_edge_size_uniform = uniforms_wrapper->get_uniforms()["vignetting_edge_size"];
                    auto current_vignetting_edge_size = vignetting_edge_size_uniform->get_f32();
                    uniforms_wrapper->set(glm::mix(current_vignetting_edge_size, glm::mix(-1.f, -.6f, motion_blur_effect_power), .1f),
                                          "vignetting_edge_size");
                }
                else if (should_show_collision_vignetting)
                {
                    uniforms_wrapper = render_technique_uniforms_component->get_uniforms("ss.compose");
                    uniforms_wrapper->set(glm::vec4(1.f, .0f, .0f, 1.f), "vignetting_color");
                    const auto vignetting_edge_size_uniform = uniforms_wrapper->get_uniforms()["vignetting_edge_size"];
                    auto current_vignetting_edge_size = vignetting_edge_size_uniform->get_f32();
                    uniforms_wrapper->set(glm::mix(current_vignetting_edge_size, glm::mix(-1.f, -.6f, 1.f - collision_power), .1f),
                                          "vignetting_edge_size");

                }
                else
                {
                    uniforms_wrapper = render_technique_uniforms_component->get_uniforms_as<ss_output_shader_uniforms>("ss.compose");
                    const auto vignetting_edge_size_uniform = uniforms_wrapper->get_uniforms()["vignetting_edge_size"];
                    auto current_vignetting_edge_size = vignetting_edge_size_uniform->get_f32();
                    current_vignetting_edge_size = glm::mix(current_vignetting_edge_size, -1.f, .1f);
                    uniforms_wrapper->set(current_vignetting_edge_size, "vignetting_edge_size");
                    uniforms_wrapper = render_technique_uniforms_component->get_uniforms_as<ss_output_shader_uniforms>("ss.compose");
                    
                    
                    
                    //const auto current_motion_direction_value = uniforms_wrapper->get_uniforms()["motion_direction"];
                    //uniforms_wrapper->set(glm::mix(current_motion_direction_value->get_vec4(),
                    //                               glm::vec4(0.f, 0.f, 0.f, 0.f),
                    //                               1.f - motion_blur_effect_power), "motion_direction");
                    
                    
                }
            }
            else
            {
                const auto uniforms_wrapper = render_technique_uniforms_component->get_uniforms("ss.compose");
                uniforms_wrapper->set(-1.f, "vignetting_edge_size");
                uniforms_wrapper->set(glm::vec4(0.f, 0.f, 0.f, 0.f), "motion_direction");
            }
        });
    }
    
    void ces_scene_visual_effects_system::on_feed_end(f32 dt)
    {
        
    }
}
