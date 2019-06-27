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

namespace game
{
    ces_scene_visual_effects_system::ces_scene_visual_effects_system()
    {
        ces_base_system::add_required_component_guid(m_scene_visual_effects_components_mask, ces_scene_visual_effects_component::class_guid());
        ces_base_system::add_required_component_guid(m_scene_visual_effects_components_mask, gb::ces_render_technique_uniforms_component::class_guid());
        ces_base_system::add_required_components_mask(m_scene_visual_effects_components_mask);
        
        ces_base_system::add_required_component_guid(m_car_components_mask, ces_car_descriptor_component::class_guid());
        ces_base_system::add_required_components_mask(m_car_components_mask);
    }
    
    void ces_scene_visual_effects_system::on_feed_start(f32 dt)
    {
        
    }
    
    void ces_scene_visual_effects_system::on_feed(const gb::ces_entity_shared_ptr& root, f32 dt)
    {
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
                const auto car = std::static_pointer_cast<gb::ces_entity>(m_main_car.lock());
                const auto car_drift_state_component = car->get_component<ces_car_drift_state_component>();
                const auto car_descriptor_component = car->get_component<ces_car_descriptor_component>();
                bool is_collided = car_drift_state_component->is_collided;
                f32 slow_motion_power = car_descriptor_component->slow_motion_power;
                bool is_in_slow_motion = slow_motion_power > 0.f;
                bool should_show_collision_vignetting = is_collided;
                bool should_show_slow_motion_vignetting = is_in_slow_motion;
                
                if (should_show_slow_motion_vignetting && !should_show_collision_vignetting)
                {
                    uniforms_wrapper = render_technique_uniforms_component->get_uniforms("ss.compose");
                    uniforms_wrapper->set(glm::vec4(.5f, 1.f, 1.f, 1.f), "vignetting_color");
                    uniforms_wrapper->set(glm::mix(-1.f, -.7f, 1.f - slow_motion_power), "vignetting_edge_size");
                }
                else if (should_show_collision_vignetting)
                {
                    uniforms_wrapper = render_technique_uniforms_component->get_uniforms("ss.compose");
                    uniforms_wrapper->set(glm::vec4(1.f, .0f, .0f, 1.f), "vignetting_color");
                    uniforms_wrapper->set(-.75f, "vignetting_edge_size");
                }
                else
                {
                    uniforms_wrapper = render_technique_uniforms_component->get_uniforms_as<ss_output_shader_uniforms>("ss.compose");
                    const auto vignetting_edge_size_uniform = uniforms_wrapper->get_uniforms()["vignetting_edge_size"];
                    auto current_vignetting_edge_size = vignetting_edge_size_uniform->get_f32();
                    current_vignetting_edge_size = glm::mix(current_vignetting_edge_size, -1.f, .1f);
                    const auto uniforms_wrapper = render_technique_uniforms_component->get_uniforms("ss.compose");
                    uniforms_wrapper->set(current_vignetting_edge_size, "vignetting_edge_size");
                }
            }
            else
            {
                const auto uniforms_wrapper = render_technique_uniforms_component->get_uniforms("ss.compose");
                uniforms_wrapper->set(-1.f, "vignetting_edge_size");
            }
        });
    }
    
    void ces_scene_visual_effects_system::on_feed_end(f32 dt)
    {
        
    }
}
