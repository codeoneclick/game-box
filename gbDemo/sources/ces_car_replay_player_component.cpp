//
//  ces_car_replay_player_component.cpp
//  gbDemo
//
//  Created by serhii.s on 4/18/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_car_replay_player_component.h"
#include "common.h"

namespace game
{
    void ces_car_replay_player_component::replay_record_data::set(const glm::vec2 &linear_velocity, f32 angular_velocity)
    {
        m_linear_velocity = linear_velocity;
        m_angular_velocity = angular_velocity;
    }
    
    glm::vec2 ces_car_replay_player_component::replay_record_data::get_linear_velocity() const
    {
        return m_linear_velocity;
    }
    
    f32 ces_car_replay_player_component::replay_record_data::get_angular_velocity() const
    {
        return m_angular_velocity;
    }
    
    void ces_car_replay_player_component::set_record(const std::unordered_map<ui32, std::shared_ptr<replay_record_data>>& record)
    {
        m_record = record;
    }
    
    void ces_car_replay_player_component::inc_tick_count()
    {
        m_tick_count++;
    }
    
    std::shared_ptr<ces_car_replay_player_component::replay_record_data> ces_car_replay_player_component::get_record_data()
    {
        std::shared_ptr<ces_car_replay_player_component::replay_record_data> result = m_last_record_data;
        const auto record_data_it = m_record.find(m_tick_count);
        if (record_data_it != m_record.end())
        {
            result = record_data_it->second;
            m_last_record_data = result;
        }
        return result;
    }
}
