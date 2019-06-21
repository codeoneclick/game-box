//
//  ces_car_tire_trails_controller_component.h
//  gbDemo
//
//  Created by serhii.s on 4/3/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_car_tire_trails_controller_component : public gb::ces_base_component
    {
    private:
        
        std::string m_filename;
        
        gb::ces_entity_weak_ptr m_tire_rl;
        gb::ces_entity_weak_ptr m_tire_rr;
        gb::scene_fabricator_weak_ptr m_scene_fabricator;
        gb::ces_entity_weak_ptr m_tire_rl_trail;
        gb::ces_entity_weak_ptr m_tire_rr_trail;
        
        std::list<gb::ces_entity_weak_ptr> m_disabled_trails;
        
    protected:
        
    public:
        
        STTI_CLASS_GUID(ces_car_tire_trails_controller_component, gb::ces_base_component::g_guids_container)
        ces_car_tire_trails_controller_component();
        ~ces_car_tire_trails_controller_component() = default;
        
        void set_parameters(const std::string& filename,
                            const gb::scene_fabricator_shared_ptr& scene_fabricator,
                            const gb::ces_entity_shared_ptr& tire_rl,
                            const gb::ces_entity_shared_ptr& tire_rr);
        
        void enable_trails();
        void disable_trails();
        
        gb::ces_entity_shared_ptr get_tire_rl_trail();
        gb::ces_entity_shared_ptr get_tire_rr_trail();

        std::list<gb::ces_entity_weak_ptr> get_disabled_trails() const;
    };
};
