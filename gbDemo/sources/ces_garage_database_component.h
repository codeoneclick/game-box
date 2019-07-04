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
                bool m_is_openned;
                i32 m_car_skin_id;
                
            protected:
                
            public:
                
                car_dto(const gb::db::database_coordinator_shared_ptr& database_coordinator);
                ~car_dto() = default;
                
                i32 get_id() const;
                i32 get_skin_id() const;
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
        i32 m_max_cars_count = 8;
        i32 m_previewed_car_id = -1;
        
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
        
        i32 get_car_unlocked_rank(i32 car_id) const;
        
        i32 get_previewed_car_id() const;
        void set_previewed_car_id(i32 id);
        
        i32 get_max_cars_count() const;
        
        void add_car_progression(i32 car_id, const std::shared_ptr<gb::car_progression_configuration>& car_progression_configuration);
        
        void update_cars_according_rank(i32 garage_id, i32 rank);
    };
};
