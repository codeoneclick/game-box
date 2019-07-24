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
#include "ces_car_camera_follow_component.h"
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
        
        ces_base_system::add_required_component_guid(m_camera_follow_components_mask, ces_car_camera_follow_component::class_guid());
        ces_base_system::add_required_components_mask(m_camera_follow_components_mask);
        
    }
    
    void ces_scene_visual_effects_system::on_feed_start(f32 dt)
    {
        
    }
    
    void ces_scene_visual_effects_system::on_feed(const gb::ces_entity_shared_ptr& root, f32 dt)
    {
        const auto camera_3d = ces_base_system::get_current_camera_3d();
        ces_base_system::enumerate_entities_with_components(m_reflection_effect_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            
            const auto shader_uniforms_component = entity->get_component<gb::ces_shader_uniforms_component>();
            const auto uniforms = shader_uniforms_component->get_uniforms(gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_vertex, "car_reflection", 2);
            if (uniforms)
            {
                const auto uniforms_set = uniforms->get_uniforms();
                const auto camera_position_uniform_it = uniforms_set.find(reflection_shader_uniforms::k_camera_position_uniform);
                if (camera_position_uniform_it != uniforms_set.end())
                {
                    uniforms->set(glm::vec4(camera_3d->get_position(), 1.f), reflection_shader_uniforms::k_camera_position_uniform);
                }
                const auto i_view_mat_uniform_it = uniforms_set.find(reflection_shader_uniforms::k_i_view_mat_uniform);
                if (i_view_mat_uniform_it != uniforms_set.end())
                {
                    uniforms->set(glm::inverse(camera_3d->get_mat_v()), reflection_shader_uniforms::k_i_view_mat_uniform);
                }
            }
        });
        
        ces_base_system::enumerate_entities_with_components(m_car_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            auto car_statistic_component = entity->get_component<ces_car_statistic_component>();
            if(car_statistic_component->mode == ces_car_statistic_component::e_mode::e_mode_player)
            {
                m_main_car = entity;
            }
        });
        
        ces_base_system::enumerate_entities_with_components(m_camera_follow_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            m_camera_follow_car = entity;
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
            
            uniforms_wrapper = render_technique_uniforms_component->get_uniforms("ss.ao");
            glm::mat4 mat_v = camera_3d->get_mat_v();
            uniforms_wrapper->set(mat_v, "mat_v");
            
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
                    glm::vec4 compose_parameters_01 = uniforms_wrapper->get_uniforms()["parameters_01"]->get_vec4();
                    auto current_vignetting_size = compose_parameters_01.x;
                    current_vignetting_size = glm::mix(current_vignetting_size, glm::mix(-1.f, -.6f, motion_blur_effect_power), .1f);
                    compose_parameters_01.x = current_vignetting_size;
                    uniforms_wrapper->set(compose_parameters_01, "parameters_01");
                }
                else if (should_show_collision_vignetting)
                {
                    uniforms_wrapper = render_technique_uniforms_component->get_uniforms("ss.compose");
                    uniforms_wrapper->set(glm::vec4(1.f, .0f, .0f, 1.f), "vignetting_color");
                    glm::vec4 compose_parameters_01 = uniforms_wrapper->get_uniforms()["parameters_01"]->get_vec4();
                    auto current_vignetting_size = compose_parameters_01.x;
                    current_vignetting_size = glm::mix(current_vignetting_size, glm::mix(-1.f, -.6f, 1.f - collision_power), .1f);
                    compose_parameters_01.x = current_vignetting_size;
                    uniforms_wrapper->set(compose_parameters_01, "parameters_01");
                }
                else
                {
                    uniforms_wrapper = render_technique_uniforms_component->get_uniforms_as<ss_output_shader_uniforms>("ss.compose");
                    glm::vec4 compose_parameters_01 = uniforms_wrapper->get_uniforms()["parameters_01"]->get_vec4();
                    auto current_vignetting_size = compose_parameters_01.x;
                    current_vignetting_size = glm::mix(current_vignetting_size, -1.f, .1f);
                    compose_parameters_01.x = current_vignetting_size;
                    uniforms_wrapper->set(compose_parameters_01, "parameters_01");
                }
            }
            else
            {
                const auto uniforms_wrapper = render_technique_uniforms_component->get_uniforms("ss.compose");
                glm::vec4 compose_parameters_01 = uniforms_wrapper->get_uniforms()["parameters_01"]->get_vec4();
                compose_parameters_01.x = -1;
                uniforms_wrapper->set(compose_parameters_01, "parameters_01");
                uniforms_wrapper->set(glm::vec4(0.f, 0.f, 0.f, 0.f), "motion_direction");
            }
            
            if (!m_camera_follow_car.expired())
            {
                const auto camera_follow_component = m_camera_follow_car.lock()->get_component<ces_car_camera_follow_component>();
                const auto uniforms_wrapper = render_technique_uniforms_component->get_uniforms("ss.compose");
                glm::vec4 compose_parameters_01 = uniforms_wrapper->get_uniforms()["parameters_01"]->get_vec4();
                ces_car_camera_follow_component::e_preview_mode current_preview_mode = camera_follow_component->preview_mode;
                if (current_preview_mode == ces_car_camera_follow_component::e_preview_mode::e_1)
                {
                    compose_parameters_01.y = 0.f;
                }
                else if (current_preview_mode == ces_car_camera_follow_component::e_preview_mode::e_2)
                {
                    compose_parameters_01.y = 1.f;
                }
                else
                {
                    compose_parameters_01.y = 0.f;
                }
                uniforms_wrapper->set(compose_parameters_01, "parameters_01");
            }
        });
    }
    
    void ces_scene_visual_effects_system::on_feed_end(f32 dt)
    {
        
    }
}
