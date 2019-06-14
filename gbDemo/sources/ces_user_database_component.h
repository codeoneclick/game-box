//
//  ces_user_database_component.h
//  gbDemo
//
//  Created by serhii.s on 6/14/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//


#include "ces_base_component.h"
#include "ns_declarations.h"
#include "db_declarations.h"

namespace game
{
    class ces_user_database_component : public gb::ces_base_component
    {
    public:
        
        class user_dto
        {
        private:
            
            friend class ces_user_database_component;
            
            gb::db::database_coordinator_weak_ptr m_database_coordinator;
            
            i32 m_id;
            i32 m_tickets = 0;
            i32 m_last_ticket_dec_timestamp = 0;
            i32 m_rank = 1;
            i32 m_stars_collected = 0;
            
        protected:
            
        public:
            
            user_dto(const gb::db::database_coordinator_shared_ptr& database_coordinator);
            ~user_dto() = default;
            
            i32 get_id() const;
            
            i32 get_tickets() const;
            
            i32 get_last_ticket_dec_timestamp() const;
            
            i32 get_rank() const;
            
            i32 get_collected_stars() const;
        };
        
        
    private:
        
        gb::db::database_coordinator_weak_ptr m_database_coordinator;
        i32 m_max_time_interval_for_ticket_generation = 3 * 60 * 1000;
        
    protected:
        
    public:
        
        STTI_CLASS_GUID(ces_user_database_component, gb::ces_base_component::g_guids_container)
        ces_user_database_component();
        ~ces_user_database_component() = default;
        
        void set_database_coordinator(const gb::db::database_coordinator_shared_ptr& database_coordinator);
        
        bool is_user_exist(i32 user_id) const;
        void add_user(i32 user_id);
        std::shared_ptr<user_dto> get_user(i32 user_id);
        
        i32 get_tickets(i32 user_id);
        
        i32 get_last_ticket_dec_timestamp(i32 user_id);
        i32 get_max_time_interval_for_ticket_generation() const;
        
        i32 get_rank(i32 user_id);
        i32 get_collected_stars(i32 user_id);
        
        void update_tickets(i32 user_id,i32 tickets);
        void inc_ticket(i32 user_id);
        void dec_ticket(i32 user_id);
        
        void update_rank(i32 user_id, i32 rank);
        void update_stars_count(i32 user_id, i32 stars);
        void inc_stars_count(i32 user_id, i32 stars);
        
        
    };
};


