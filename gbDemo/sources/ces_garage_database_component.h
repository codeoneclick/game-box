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
                i32 m_car_index;
                bool m_is_openned;
                i32 m_car_skin_index;
                
                protected:
                
                public:
                
                car_dto(const gb::db::database_coordinator_shared_ptr& database_coordinator);
                ~car_dto() = default;
                
                i32 get_id() const;
                i32 get_skin_index() const;
            };
            
            private:
            
            friend class ces_garage_database_component;
            
            gb::db::database_coordinator_weak_ptr m_database_coordinator;
            
            i32 m_id;
            i32 m_selected_car;
            std::unordered_map<i32, std::shared_ptr<car_dto>> m_cars;
            
            protected:
            
            public:
            
            garage_dto(const gb::db::database_coordinator_shared_ptr& database_coordinator);
            ~garage_dto() = default;;
        };
        
        private:
        
        gb::db::database_coordinator_weak_ptr m_database_coordinator;
        
        protected:
        
        public:
        
        CTTI_CLASS_GUID(ces_garage_database_component, gb::ces_base_component::g_guids_container)
        ces_garage_database_component();
        ~ces_garage_database_component() = default;
        
        void set_database_coordinator(const gb::db::database_coordinator_shared_ptr& database_coordinator);
        
        bool is_garage_exist(i32 garage_id) const;
        void add_garage(i32 garage_id);
        bool is_car_exist(i32 garage_id, i32 car_id) const;
        void add_car_to_garage(i32 garage_id, i32 car_id);
        
        void select_car(i32 garage_id, i32 car_id);
        void select_car_skin(i32 garage_id, i32 car_id, i32 car_skin_index);
        std::unordered_map<i32, std::shared_ptr<garage_dto::car_dto>> get_all_cars() const;
        std::shared_ptr<garage_dto::car_dto> get_car(i32 car_index);
        
        std::shared_ptr<garage_dto::car_dto> get_selected_car(i32 garage_id);
    };
};
