//
//  ces_car_descriptor_component.cpp
//  gbDemo
//
//  Created by serhii.s on 2/15/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_car_descriptor_component.h"

namespace game
{
    
    bool ces_car_descriptor_component::car_upgrade::is_equal(const std::shared_ptr<car_upgrade>& other)
    {
        return m_car_body_color_id == other->m_car_body_color_id &&
        m_car_windshield_color_id == other->m_car_windshield_color_id &&
        m_car_speed_upgrade_value == other->m_car_speed_upgrade_value &&
        m_car_handling_upgrade_value == other->m_car_handling_upgrade_value &&
        m_car_rigidity_upgrade_value == other->m_car_rigidity_upgrade_value;
    }
    
    void ces_car_descriptor_component::car_upgrade::apply(const std::shared_ptr<car_upgrade>& other)
    {
        m_car_body_color_id = other->m_car_body_color_id;
        m_car_windshield_color_id = other->m_car_windshield_color_id;
        m_car_speed_upgrade_value = other->m_car_speed_upgrade_value;
        m_car_handling_upgrade_value = other->m_car_handling_upgrade_value;
        m_car_rigidity_upgrade_value = other->m_car_rigidity_upgrade_value;
    }
    
    ces_car_descriptor_component::ces_car_descriptor_component()
    {
        position_wc.getter([=]() {
            return m_position_wc;
        });
        
        position_wc.setter([=](const glm::vec2& position_wc) {
            m_position_wc = position_wc;
        });
        
        velocity_wc.getter([=]() {
            return m_velocity_wc;
        });
        
        velocity_wc.setter([=](const glm::vec2& velocity_wc) {
            m_velocity_wc = velocity_wc;
        });
        
        angular_velocity.getter([=]() {
            return m_angular_velocity;
        });
        
        angular_velocity.setter([=](f32 angular_velocity) {
            m_angular_velocity = angular_velocity;
        });
        
        steer_angle.getter([=]() {
            return m_steer_angle;
        });
        
        steer_angle.setter([=](f32 steer_angle) {
            m_steer_angle = steer_angle;
        });
        
        throttle.getter([=]() {
            return m_throttle;
        });
        
        throttle.setter([=](f32 throttle) {
            m_throttle = throttle;
        });
        
        brake.getter([=]() {
            return m_brake;
        });
        
        brake.setter([=](f32 brake) {
            m_brake = brake;
        });
        
        side_angle.getter([=]() {
            return m_side_angle;
        });
        
        side_angle.setter([=](const glm::interpolated_f32& side_angle) {
            m_side_angle = side_angle;
        });
        
        body_angle.getter([=]() {
            return m_body_angle;
        });
        
        body_angle.setter([=](const glm::interpolated_f32& body_angle) {
            m_body_angle = body_angle;
        });
        
        last_collided_timestamp.getter([=]() {
            return m_last_collided_timestamp;
        });
        
        last_collided_timestamp.setter([=](f32 value) {
            m_last_collided_timestamp = value;
        });
        
        max_colission_interval.getter([=]() {
            return m_max_colission_interval;
        });
        
        lap_passed_index.getter([=]() {
            return m_lap_passed_index;
        });
        
        lap_passed_index.setter([=](i32 value) {
            m_lap_passed_index = value;
        });
        
        checkpoint_passed_index.getter([=]() {
            return m_checkpoint_passed_index;
        });
        
        checkpoint_passed_index.setter([=](i32 value) {
            m_checkpoint_passed_index = value;
        });
        
        distance_to_next_checkpoint.getter([=]() {
            return m_distance_to_next_checkpoint;
        });
        
        distance_to_next_checkpoint.setter([=](f32 value) {
            m_distance_to_next_checkpoint = value;
        });
        
        racer_name.getter([=]() {
            return m_racer_name;
        });
        
        racer_name.setter([=](const std::string& value) {
            m_racer_name = value;
        });
        
        place.getter([=]() {
            return m_place;
        });
        
        place.setter([=](i32 value) {
            m_place = value;
        });
        
        nearest_slow_motion_trigger_index.getter([=]() {
            return m_nearest_slow_motion_trigger_index;
        });
        
        nearest_slow_motion_trigger_index.setter([=](f32 value) {
            m_nearest_slow_motion_trigger_index = value;
        });
        
        distance_to_activate_motion_trigger.getter([=]() {
            return m_distance_to_activate_motion_trigger;
        });
        
        slow_motion_power.getter([=]() {
            return m_slow_motion_power;
        });
        
        slow_motion_power.setter([=](f32 value) {
            m_slow_motion_power = value;
        });
        
        current_damage.getter([=]() {
            return m_current_damage;
        });
        
        current_damage.setter([=](f32 value) {
            m_current_damage = value;
        });
        
        max_damage.getter([=]() {
            return m_max_damage;
        });
        
        max_damage.setter([=](f32 value) {
            m_max_damage = value;
        });
        
        motion_blur_effect_power.getter([=]() {
            return m_motion_blur_effect_power;
        });
        
        motion_blur_effect_power.setter([=](f32 value) {
            m_motion_blur_effect_power = value;
        });
    }
    
    std::shared_ptr<ces_car_descriptor_component::car_upgrade> ces_car_descriptor_component::get_car_upgrade() const
    {
        return m_car_upgrade;
    }
    
    std::shared_ptr<ces_car_descriptor_component::car_upgrade> ces_car_descriptor_component::get_car_upgrade_cache() const
    {
        return m_car_upgrade_cache;
    }
}
