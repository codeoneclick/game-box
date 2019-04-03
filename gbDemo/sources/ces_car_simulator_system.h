//
//  ces_car_simulator_system.hpp
//  gbDemo
//
//  Created by serhii.s on 2/15/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_system.h"

namespace game
{
    class ces_car_simulator_system : public gb::ces_base_system
    {
    private:
        
        std::bitset<std::numeric_limits<uint8_t>::max()> m_car_simulator_components_mask;
        
        void update_car_lights_direction(const gb::ces_entity_shared_ptr& entity);
        void update_car_drift_state(const gb::ces_entity_shared_ptr& entity);
        void update_car_tire_particles(const gb::ces_entity_shared_ptr& entity);
        void update_car_tire_trails(const gb::ces_entity_shared_ptr& entity);
        
    protected:
        
        void on_feed_start(f32 dt);
        void on_feed(const gb::ces_entity_shared_ptr& root, f32 dt);
        void on_feed_end(f32 dt);
        
    public:
        
        CTTI_CLASS_GUID(ces_car_simulator_system, gb::ces_base_system::g_guids_container)
        ces_car_simulator_system();
        ~ces_car_simulator_system();
    };
};
