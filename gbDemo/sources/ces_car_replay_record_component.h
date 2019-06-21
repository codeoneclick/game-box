//
//  ces_car_replay_record_component.h
//  gbDemo
//
//  Created by serhii.s on 4/17/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_car_replay_record_component : public gb::ces_base_component
    {
    public:
        
        class replay_record_data
        {
        private:
            
            glm::vec2 m_linear_velocity = glm::vec2(0.f);
            f32 m_angular_velocity = 0.f;
            
        protected:
            
        public:
            
            replay_record_data() = default;
            ~replay_record_data() = default;
            
            void set(const glm::vec2& linear_velocity, f32 angular_velocity);
            
            glm::vec2 get_linear_velocity() const;
            f32 get_angular_velocity() const;
        };
        
    private:
        
        static const ui32 k_max_records_count;
        
        ui32 m_tick_count = 0;
        std::string m_guid;
        std::unordered_map<ui32, std::shared_ptr<replay_record_data>> m_record;
        bool m_is_saved = false;
        
    protected:
        
    public:
        
        STTI_CLASS_GUID(ces_car_replay_record_component, gb::ces_base_component::g_guids_container)
        ces_car_replay_record_component() = default;
        ~ces_car_replay_record_component() = default;
        
        void set_record_guid(const std::string& guid);
        
        void inc_tick_count();
        
        void record(const glm::vec2& linear_velocity, f32 angular_velocity);
    };
};
