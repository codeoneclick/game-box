//
//  ces_garage_database_component.h
//  gbDemo
//
//  Created by serhii.s on 5/16/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"
#include "db_declarations.h"
#include "car_progression_configuration.h"

namespace game
{
    class ces_garage_database_component : public gb::ces_base_component
    {
    public:
        
        class garage_dto
        {
        public:
            
            class car_dto
            {
            private:
                
                friend class ces_garage_database_component;
                
                gb::db::database_coordinator_weak_ptr m_database_coordinator;
                
                i32 m_id;
                i32 m_garage_id;
                i32 m_is_openned;
                i32 m_is_bought;
                i32 m_price;
                i32 m_car_body_color_id;
                i32 m_car_windows_color_id;
                i32 m_car_speed_upgrade;
                i32 m_car_handling_upgrade;
                i32 m_car_rigidity_upgrade;
                
            protected:
                
            public:
                
                car_dto(const gb::db::database_coordinator_shared_ptr& database_coordinator);
                ~car_dto() = default;
                
                i32 get_id() const;
                bool get_is_openned() const;
                bool get_is_bought() const;
                i32 get_price() const;
                i32 get_car_body_color_id() const;
                i32 get_car_windows_color_id() const;
                f32 get_car_speed_upgrade() const;
                f32 get_car_handling_upgrade() const;
                f32 get_car_rigidity_upgrade() const;
            };
            
        private:
            
            friend class ces_garage_database_component;
            
            gb::db::database_coordinator_weak_ptr m_database_coordinator;
            
            i32 m_id;
            i32 m_selected_car_id;
            std::unordered_map<i32, std::shared_ptr<car_dto>> m_cars;
            
        protected:
            
        public:
            
            garage_dto(const gb::db::database_coordinator_shared_ptr& database_coordinator);
            ~garage_dto() = default;;
        };
        
    private:
        
        gb::db::database_coordinator_weak_ptr m_database_coordinator;
        i32 m_max_cars_count = 13;
        i32 m_previewed_car_id = -1;
        
        const i32 m_initial_price_for_color_switch = 100;
        const f32 m_price_for_color_switch_increase_coefficient = 1.33f;
        
        const i32 m_initial_price_for_performance_upgrade = 100;
        const f32 m_price_for_performance_upgrade_increase_coefficient_per_car = 1.66f;
        const f32 m_price_for_performance_upgrade_increase_coefficient_per_upgrade = 2.f;
        
        std::unordered_map<ui32, std::shared_ptr<gb::car_progression_configuration>> m_cars_progression_configurations;
        
    protected:
        
    public:
        
        STTI_CLASS_GUID(ces_garage_database_component, gb::ces_base_component::g_guids_container)
        ces_garage_database_component();
        ~ces_garage_database_component() = default;
        
        void set_database_coordinator(const gb::db::database_coordinator_shared_ptr& database_coordinator);
        
        bool is_garage_exist(i32 garage_id) const;
        void add_garage(i32 garage_id);
        bool is_car_exist(i32 garage_id, i32 car_id) const;
        void add_car_to_garage(i32 garage_id, i32 car_id);
        
        std::unordered_map<i32, std::shared_ptr<garage_dto::car_dto>> get_all_cars() const;
        std::shared_ptr<garage_dto::car_dto> get_car(i32 garage_id, i32 car_id);
        
        void select_car(i32 garage_id, i32 car_id);
        std::shared_ptr<garage_dto::car_dto> get_selected_car(i32 garage_id);
        void select_car_skin(i32 garage_id, i32 car_id, i32 car_skin_id);
    
        void open_car(i32 garage_id, i32 car_id);
        void close_car(i32 garage_id, i32 car_id);
        bool is_car_oppenned(i32 garage_id, i32 car_id) const;
        
        void buy_car(i32 garage_id, i32 car_id);
        
        i32 get_car_unlocked_rank(i32 car_id) const;
        
        i32 get_previewed_car_id() const;
        void set_previewed_car_id(i32 id);
        
        i32 get_max_cars_count() const;
        
        void add_car_progression(i32 car_id, const std::shared_ptr<gb::car_progression_configuration>& car_progression_configuration);
        
        void update_cars_according_rank(i32 garage_id, i32 rank);
        
        i32 get_price_for_color_switch(i32 garage_id, i32 car_id);
        i32 get_price_for_speed_upgrade(i32 garage_id, i32 car_id, f32 delta);
        i32 get_price_for_handling_upgrade(i32 garage_id, i32 car_id, f32 delta);
        i32 get_price_for_durability_upgrade(i32 garage_id, i32 car_id, f32 delta);
        
        void set_car_body_color_id(i32 garage_id, i32 car_id, i32 color_id);
        void set_car_windshield_color_id(i32 garage_id, i32 car_id, i32 color_id);
        
        void upgrade_car_speed(i32 garage_id, i32 car_id, f32 delta);
        void upgrade_car_handling(i32 garage_id, i32 car_id, f32 delta);
        void upgrade_car_durability(i32 garage_id, i32 car_id, f32 delta);
    };
};
