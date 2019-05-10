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

namespace game
{
    ces_scene_visual_effects_system::ces_scene_visual_effects_system()
    {
        ces_base_system::add_required_component_guid(m_scene_visual_effects_components_mask, ces_scene_visual_effects_component::class_guid());
        ces_base_system::add_required_component_guid(m_scene_visual_effects_components_mask, gb::ces_render_technique_uniforms_component::class_guid());
        ces_base_system::add_required_components_mask(m_scene_visual_effects_components_mask);
    }
    
    void ces_scene_visual_effects_system::on_feed_start(f32 dt)
    {
        
    }
    
    void ces_scene_visual_effects_system::on_feed(const gb::ces_entity_shared_ptr& root, f32 dt)
    {
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
        });
    }
    
    void ces_scene_visual_effects_system::on_feed_end(f32 dt)
    {
        
    }
}
