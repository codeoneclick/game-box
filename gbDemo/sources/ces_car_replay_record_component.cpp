//
//  ces_car_replay_record_component.cpp
//  gbDemo
//
//  Created by serhii.s on 4/17/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_car_replay_record_component.h"
#include "common.h"

namespace game
{
    const ui32 ces_car_replay_record_component::k_max_records_count = 10000;
    
    
    void ces_car_replay_record_component::replay_record_data::set(const glm::vec2 &linear_velocity, f32 angular_velocity)
    {
        m_linear_velocity = linear_velocity;
        m_angular_velocity = angular_velocity;
    }
    
    glm::vec2 ces_car_replay_record_component::replay_record_data::get_linear_velocity() const
    {
        return m_linear_velocity;
    }
    
    f32 ces_car_replay_record_component::replay_record_data::get_angular_velocity() const
    {
        return m_angular_velocity;
    }
    
    void ces_car_replay_record_component::inc_tick_count()
    {
        m_tick_count++;
    }
    
    void ces_car_replay_record_component::set_record_guid(const std::string& guid)
    {
        m_guid = guid;
    }
    
    void ces_car_replay_record_component::record(const glm::vec2& linear_velocity, f32 angular_velocity)
    {
        if (m_record.size() < k_max_records_count)
        {
            const auto replay_record_data = std::make_shared<ces_car_replay_record_component::replay_record_data>();
            replay_record_data->set(linear_velocity, angular_velocity);
            m_record[m_tick_count] = replay_record_data;
        }
        else if (!m_is_saved)
        {
            std::string filename = documentspath();
            filename.append(m_guid);
            filename.append(".rep");
            std::ofstream outstream;
            outstream.open(filename.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
            ui32 records_count = static_cast<ui32>(m_record.size());
            outstream.write((char*)&records_count, sizeof(ui32));
            for (auto record_data : m_record)
            {
                ui32 tick = record_data.first;
                outstream.write((char*)&tick, sizeof(ui32));
                glm::vec2 linear_velocity = record_data.second->get_linear_velocity();
                outstream.write((char*)&linear_velocity, sizeof(glm::vec2));
                f32 angular_velocity = record_data.second->get_angular_velocity();
                outstream.write((char*)&angular_velocity, sizeof(f32));
            }
            outstream.close();
            m_is_saved = true;
        }
    }
}
