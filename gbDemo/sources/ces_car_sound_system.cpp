//
//  ces_car_sound_system.cpp
//  gbDemo
//
//  Created by serhii.s on 4/19/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_car_sound_system.h"
#include "ces_car_sounds_set_component.h"
#include "ces_car_fuzzy_logic_component.h"
#include "ces_car_descriptor_component.h"
#include "ces_car_model_component.h"
#include "ces_sound_component.h"
#include "ces_car_drift_state_component.h"
#include "ces_car_gear_component.h"
#include "glm_extensions.h"

namespace game
{
    ces_car_sound_system::ces_car_sound_system()
    {
        ces_base_system::add_required_component_guid(m_car_sound_components_mask, ces_car_sounds_set_component::class_guid());
        ces_base_system::add_required_component_guid(m_car_sound_components_mask, ces_car_fuzzy_logic_component::class_guid());
        ces_base_system::add_required_components_mask(m_car_sound_components_mask);
    }
    
    ces_car_sound_system::~ces_car_sound_system()
    {
        
    }
    
    void ces_car_sound_system::on_feed_start(f32 dt)
    {
        
    }
    
    void ces_car_sound_system::on_feed(const gb::ces_entity_shared_ptr& root, f32 dt)
    {
        ces_base_system::enumerate_entities_with_components(m_car_sound_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            
            const auto car_sounds_set_component = entity->get_component<ces_car_sounds_set_component>();
            const auto car_fuzzy_logic_component = entity->get_component<ces_car_fuzzy_logic_component>();
            const auto car_descriptor_component = entity->get_component<ces_car_descriptor_component>();
            const auto car_model_component = entity->get_component<ces_car_model_component>();
            const auto sound_component = entity->get_component<gb::al::ces_sound_component>();
            const auto car_drift_state_component = entity->get_component<ces_car_drift_state_component>();
            const auto car_gear_component = entity->get_component<ces_car_gear_component>();
            
            glm::vec2 velocity_wc = car_descriptor_component->velocity_wc;
            f32 velocity_wc_length = glm::length(velocity_wc);
            f32 current_velocity_length_squared = velocity_wc_length * velocity_wc_length;
            f32 max_speed_squared = car_model_component->get_max_speed() * car_model_component->get_max_speed();
            f32 current_speed_factor = glm::clamp(current_velocity_length_squared / max_speed_squared, 0.f, 1.f);
            
            /*i32 throttle = car_gear_component->get_throttle();
            throttle += 8;
            throttle = glm::fixup(throttle);
            throttle = glm::clamp(throttle, 0, 100);
            car_gear_component->set_throttle(throttle);*/
            i32 throttle = car_descriptor_component->throttle;
            
            i32 rpm = car_gear_component->get_rpm(current_speed_factor, car_gear_component->get_previous_load());
            
            ffll_set_value(car_fuzzy_logic_component->get_engine_load_script(),
                           car_fuzzy_logic_component->get_engine_load_script_executor(), 0, throttle);
            ffll_set_value(car_fuzzy_logic_component->get_engine_load_script(),
                           car_fuzzy_logic_component->get_engine_load_script_executor(), 1, rpm);
            
            i32 engine_load = (i32)ffll_get_output_value(car_fuzzy_logic_component->get_engine_load_script(),
                                                         car_fuzzy_logic_component->get_engine_load_script_executor());
            car_gear_component->get_rpm(current_speed_factor, engine_load);
            
            // IDLE
            ffll_set_value(car_fuzzy_logic_component->get_engine_volume_idle_script(),
                           car_fuzzy_logic_component->get_engine_volume_idle_script_executor(), 0, engine_load);
            ffll_set_value(car_fuzzy_logic_component->get_engine_volume_idle_script(),
                           car_fuzzy_logic_component->get_engine_volume_idle_script_executor(), 1, rpm);
            
            f32 engine_idle_volume = (f32)ffll_get_output_value(car_fuzzy_logic_component->get_engine_volume_idle_script(),
                                                                car_fuzzy_logic_component->get_engine_volume_idle_script_executor());
            engine_idle_volume /= 100.f;
            
            //OFF LOW
            
            ffll_set_value(car_fuzzy_logic_component->get_engine_volume_off_low_script(),
                           car_fuzzy_logic_component->get_engine_volume_off_low_script_executor(), 0, engine_load);
            ffll_set_value(car_fuzzy_logic_component->get_engine_volume_off_low_script(),
                           car_fuzzy_logic_component->get_engine_volume_off_low_script_executor(), 1, rpm);
            
            f32 engine_off_low_volume = (f32)ffll_get_output_value(car_fuzzy_logic_component->get_engine_volume_off_low_script(),
                                                                  car_fuzzy_logic_component->get_engine_volume_off_low_script_executor());
            engine_off_low_volume /= 100.f;
            
            //OFF MID
            
            ffll_set_value(car_fuzzy_logic_component->get_engine_volume_off_mid_script(),
                           car_fuzzy_logic_component->get_engine_volume_off_mid_script_executor(), 0, engine_load);
            ffll_set_value(car_fuzzy_logic_component->get_engine_volume_off_mid_script(),
                           car_fuzzy_logic_component->get_engine_volume_off_mid_script_executor(), 1, rpm);
            
            f32 engine_off_mid_volume = (f32)ffll_get_output_value(car_fuzzy_logic_component->get_engine_volume_off_mid_script(),
                                                                  car_fuzzy_logic_component->get_engine_volume_off_mid_script_executor());
            engine_off_mid_volume /= 100.f;
            
            // OFF HIGH
            
            ffll_set_value(car_fuzzy_logic_component->get_engine_volume_off_high_script(),
                           car_fuzzy_logic_component->get_engine_volume_off_high_script_executor(), 0, engine_load);
            ffll_set_value(car_fuzzy_logic_component->get_engine_volume_off_high_script(),
                           car_fuzzy_logic_component->get_engine_volume_off_high_script_executor(), 1, rpm);
            
            f32 engine_off_high_volume = (f32)ffll_get_output_value(car_fuzzy_logic_component->get_engine_volume_off_high_script(),
                                                                  car_fuzzy_logic_component->get_engine_volume_off_high_script_executor());
            engine_off_high_volume /= 100.f;
            
            //ON LOW
            
            ffll_set_value(car_fuzzy_logic_component->get_engine_volume_on_low_script(),
                           car_fuzzy_logic_component->get_engine_volume_on_low_script_executor(), 0, engine_load);
            ffll_set_value(car_fuzzy_logic_component->get_engine_volume_on_low_script(),
                           car_fuzzy_logic_component->get_engine_volume_on_low_script_executor(), 1, rpm);
            
            f32 engine_on_low_volume = (f32)ffll_get_output_value(car_fuzzy_logic_component->get_engine_volume_on_low_script(),
                                                                  car_fuzzy_logic_component->get_engine_volume_on_low_script_executor());
            engine_on_low_volume /= 100.f;
            
            //ON MID
            
            ffll_set_value(car_fuzzy_logic_component->get_engine_volume_on_mid_script(),
                           car_fuzzy_logic_component->get_engine_volume_on_mid_script_executor(), 0, engine_load);
            ffll_set_value(car_fuzzy_logic_component->get_engine_volume_on_mid_script(),
                           car_fuzzy_logic_component->get_engine_volume_on_mid_script_executor(), 1, rpm);
            
            f32 engine_on_mid_volume = (f32)ffll_get_output_value(car_fuzzy_logic_component->get_engine_volume_on_mid_script(),
                                                                   car_fuzzy_logic_component->get_engine_volume_on_mid_script_executor());
            engine_on_mid_volume /= 100.f;
            
            // ON HIGH
            
            ffll_set_value(car_fuzzy_logic_component->get_engine_volume_on_high_script(),
                           car_fuzzy_logic_component->get_engine_volume_on_high_script_executor(), 0, engine_load);
            ffll_set_value(car_fuzzy_logic_component->get_engine_volume_on_high_script(),
                           car_fuzzy_logic_component->get_engine_volume_on_high_script_executor(), 1, rpm);
            
            f32 engine_on_high_volume = (f32)ffll_get_output_value(car_fuzzy_logic_component->get_engine_volume_on_high_script(),
                                                                    car_fuzzy_logic_component->get_engine_volume_on_high_script_executor());
            engine_on_high_volume /= 100.f;
            
            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_idle, engine_idle_volume);
            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_off_low, engine_off_low_volume);
            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_off_mid, engine_off_mid_volume);
            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_off_high, engine_off_high_volume);
            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_on_low, engine_on_low_volume);
            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_on_mid, engine_on_mid_volume);
            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_on_high, engine_on_high_volume);
            
            if (car_drift_state_component->is_drifting)
            {
                sound_component->set_volume(ces_car_sounds_set_component::sounds::k_drift, 1.f);
            }
            else
            {
                sound_component->set_volume(ces_car_sounds_set_component::sounds::k_drift, glm::mix(sound_component->get_volume(ces_car_sounds_set_component::sounds::k_drift), 0.f, .1f));
            }
            
            f32 low_limit = 6000.f;
            f32 high_limit = 10000.f;
            
            f32 rpm_low = rpm / low_limit;
            f32 rpm_high = rpm / high_limit;
            
            f32 from = 0.55f;
            f32 to = 0.05f;
            f32 amount = from - (from - to) * current_speed_factor;

            if (rpm < low_limit)
            {
                sound_component->set_pitch(ces_car_sounds_set_component::sounds::k_engine_idle, std::pow(2, rpm_low - amount));
                sound_component->set_pitch(ces_car_sounds_set_component::sounds::k_engine_on_low, std::pow(2, rpm_high + .2f - amount));
            }
            
            sound_component->set_pitch(ces_car_sounds_set_component::sounds::k_engine_on_mid, std::pow(2, rpm_high - .4f - amount));
            sound_component->set_pitch(ces_car_sounds_set_component::sounds::k_engine_on_high, std::pow(2, rpm_high - .8f - amount));
            sound_component->set_pitch(ces_car_sounds_set_component::sounds::k_engine_off_low, std::pow(2, rpm_high - .2f - amount));
            sound_component->set_pitch(ces_car_sounds_set_component::sounds::k_engine_off_mid, std::pow(2, rpm_high - .4f - amount));
            sound_component->set_pitch(ces_car_sounds_set_component::sounds::k_engine_off_high, std::pow(2, rpm_high - .8f - amount));
        });
    }
    
    void ces_car_sound_system::on_feed_end(f32 dt)
    {
        
    }
}
