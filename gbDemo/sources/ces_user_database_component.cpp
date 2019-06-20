//
//  ces_user_database_component.cpp
//  gbDemo
//
//  Created by serhii.s on 6/14/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_user_database_component.h"
#include "db_user_table.h"
#include "database_entity.h"

namespace game
{
    ces_user_database_component::user_dto::user_dto(const gb::db::database_coordinator_shared_ptr& database_coordinator) :
    m_database_coordinator(database_coordinator)
    {
        
    }
    
    i32 ces_user_database_component::user_dto::get_id() const
    {
        return m_id;
    }
    
    i32 ces_user_database_component::user_dto::get_rank() const
    {
        return m_rank;
    }
    
    i32 ces_user_database_component::user_dto::get_claimed_rank() const
    {
        return m_claimed_rank;
    }
    
    i32 ces_user_database_component::user_dto::get_collected_stars() const
    {
        return m_stars_collected;
    }
    
    i32 ces_user_database_component::user_dto::get_tickets() const
    {
        return m_tickets;
    }
    
    i32 ces_user_database_component::user_dto::get_last_ticket_dec_timestamp() const
    {
        return m_last_ticket_dec_timestamp;
    }
    
    ces_user_database_component::ces_user_database_component()
    {
        
    }
    
    void ces_user_database_component::set_database_coordinator(const gb::db::database_coordinator_shared_ptr& database_coordinator)
    {
        m_database_coordinator = database_coordinator;
    }
    
    bool ces_user_database_component::is_user_exist(i32 user_id) const
    {
        auto user_record = std::make_shared<gb::db::database_entity<db_user_table, db_user_data>>(m_database_coordinator.lock());
        return user_record->load_from_db(user_id);
    }
    
    void ces_user_database_component::add_user(i32 user_id)
    {
        auto user_record = std::make_shared<gb::db::database_entity<db_user_table, db_user_data>>(m_database_coordinator.lock());
        if(!user_record->load_from_db(user_id))
        {
            auto& data = user_record->get_data();
            data.m_id = user_id;
            data.m_rank = 1;
            data.m_claimed_rank = 1;
            data.m_tickets = 5;
            data.m_stars_collected = 0;
            user_record->save_to_db();
        }
    }
    
    std::shared_ptr<ces_user_database_component::user_dto> ces_user_database_component::get_user(i32 user_id)
    {
        std::shared_ptr<ces_user_database_component::user_dto> user_dto = nullptr;
        auto user_record = std::make_shared<gb::db::database_entity<db_user_table, db_user_data>>(m_database_coordinator.lock());
        if(!user_record->load_from_db(user_id))
        {
            assert(false);
        }
        else
        {
            user_dto = std::make_shared<ces_user_database_component::user_dto>(m_database_coordinator.lock());
            user_dto->m_id = user_record->get_data().m_id;
            user_dto->m_tickets = user_record->get_data().m_tickets;
            user_dto->m_rank = user_record->get_data().m_rank;
            user_dto->m_claimed_rank = user_record->get_data().m_claimed_rank;
            user_dto->m_stars_collected = user_record->get_data().m_stars_collected;
            user_dto->m_last_ticket_dec_timestamp = user_record->get_data().m_last_ticket_dec_timestamp;
        }
        return user_dto;
    
    }
    
    i32 ces_user_database_component::get_tickets(i32 user_id)
    {
        i32 result = 0;
        const auto user = get_user(user_id);
        if (user)
        {
            result = user->get_tickets();
        }
        return result;
    }
    
    i32 ces_user_database_component::get_last_ticket_dec_timestamp(i32 user_id)
    {
        i32 result = 0;
        const auto user = get_user(user_id);
        if (user)
        {
            result = user->get_last_ticket_dec_timestamp();
        }
        return result;
    }
    
    i32 ces_user_database_component::get_rank(i32 user_id)
    {
        i32 result = 0;
        const auto user = get_user(user_id);
        if (user)
        {
            result = user->get_rank();
        }
        return result;
    }
    
    i32 ces_user_database_component::get_claimed_rank(i32 user_id)
    {
        i32 result = 0;
        const auto user = get_user(user_id);
        if (user)
        {
            result = user->get_claimed_rank();
        }
        return result;
    }
    
    i32 ces_user_database_component::get_collected_stars(i32 user_id)
    {
        i32 result = 0;
        const auto user = get_user(user_id);
        if (user)
        {
            result = user->get_collected_stars();
        }
        return result;
    }
    
    void ces_user_database_component::update_tickets(i32 user_id, i32 tickets)
    {
        auto user_record = std::make_shared<gb::db::database_entity<db_user_table, db_user_data>>(m_database_coordinator.lock());
        if(!user_record->load_from_db(user_id))
        {
            assert(false);
        }
        else
        {
            auto& data = user_record->get_data();
            data.m_tickets = std::max(0, tickets);
            if (data.m_tickets < 5)
            {
                data.m_last_ticket_dec_timestamp = static_cast<i32>(std::get_tick_count());
            }
            else
            {
                data.m_last_ticket_dec_timestamp = 0;
            }
            user_record->save_to_db();
        }
    }
    
    void ces_user_database_component::inc_ticket(i32 user_id)
    {
        auto user_record = std::make_shared<gb::db::database_entity<db_user_table, db_user_data>>(m_database_coordinator.lock());
        if(!user_record->load_from_db(user_id))
        {
            assert(false);
        }
        else
        {
            auto& data = user_record->get_data();
            data.m_tickets += 1;
            if (data.m_tickets >= 5)
            {
                data.m_last_ticket_dec_timestamp = 0;
            }
            user_record->save_to_db();
        }
    }
    
    void ces_user_database_component::dec_ticket(i32 user_id)
    {
        auto user_record = std::make_shared<gb::db::database_entity<db_user_table, db_user_data>>(m_database_coordinator.lock());
        if(!user_record->load_from_db(user_id))
        {
            assert(false);
        }
        else
        {
            auto& data = user_record->get_data();
            data.m_tickets -= 1;
            data.m_tickets = std::max(0, data.m_tickets);
            if (data.m_last_ticket_dec_timestamp == 0 && data.m_tickets < 5)
            {
                data.m_last_ticket_dec_timestamp = static_cast<i32>(std::get_tick_count());
            }
            user_record->save_to_db();
        }
    }
    
    void ces_user_database_component::update_rank(i32 user_id, i32 rank)
    {
        auto user_record = std::make_shared<gb::db::database_entity<db_user_table, db_user_data>>(m_database_coordinator.lock());
        if(!user_record->load_from_db(user_id))
        {
            assert(false);
        }
        else
        {
            auto& data = user_record->get_data();
            data.m_rank = rank;
            user_record->save_to_db();
        }
    }
    
    void ces_user_database_component::update_claimed_rank(i32 user_id)
    {
        auto user_record = std::make_shared<gb::db::database_entity<db_user_table, db_user_data>>(m_database_coordinator.lock());
        if(!user_record->load_from_db(user_id))
        {
            assert(false);
        }
        else
        {
            auto& data = user_record->get_data();
            data.m_claimed_rank = data.m_rank;
            user_record->save_to_db();
        }
    }
    
    void ces_user_database_component::update_stars_count(i32 user_id, i32 stars)
    {
        auto user_record = std::make_shared<gb::db::database_entity<db_user_table, db_user_data>>(m_database_coordinator.lock());
        if(!user_record->load_from_db(user_id))
        {
            assert(false);
        }
        else
        {
            auto& data = user_record->get_data();
            data.m_stars_collected = stars;
            user_record->save_to_db();
        }
    }
    
    void ces_user_database_component::inc_stars_count(i32 user_id, i32 stars)
    {
        auto user_record = std::make_shared<gb::db::database_entity<db_user_table, db_user_data>>(m_database_coordinator.lock());
        if(!user_record->load_from_db(user_id))
        {
            assert(false);
        }
        else
        {
            auto& data = user_record->get_data();
            data.m_stars_collected += stars;
            user_record->save_to_db();
        }
    }
    
    i32 ces_user_database_component::get_collected_stars(i32 user_id, i32 rank)
    {
        i32 stars_count = get_collected_stars(user_id);
        for (const auto& car_progression_it : m_cars_progression_configurations)
        {
            if (car_progression_it.second->get_required_rank() < rank)
            {
                stars_count -= car_progression_it.second->get_required_stars_for_next_rank();
            }
        }
        
        return stars_count > 0 ? stars_count : 0;
    }
    
    i32 ces_user_database_component::get_stars_for_rank(i32 rank)
    {
        i32 result = 0;
        
        for (const auto& car_progression_it : m_cars_progression_configurations)
        {
            if (car_progression_it.second->get_required_rank() == (rank - 1))
            {
                result = car_progression_it.second->get_required_stars_for_next_rank();
            }
        }
        
        return result;
    }
    
    i32 ces_user_database_component::get_max_time_interval_for_ticket_generation() const
    {
        return m_max_time_interval_for_ticket_generation;
    }
    
    void ces_user_database_component::add_car_progression(i32 car_id, const std::shared_ptr<gb::car_progression_configuration>& car_progression_configuration)
    {
        m_cars_progression_configurations[car_id] = car_progression_configuration;
        if (m_max_rank < car_progression_configuration->get_required_rank())
        {
            m_max_rank = car_progression_configuration->get_required_rank();
        }
    }
    
    void ces_user_database_component::set_user_start_level_with_rank(i32 user_id, i32 rank)
    {
        m_user_start_level_with_rank = rank;
    }
    
    i32 ces_user_database_component::get_user_start_level_with_rank(i32 user_id)
    {
        return m_user_start_level_with_rank;
    }
    
    void ces_user_database_component::update_rank_according_stars_count(i32 user_id)
    {
        i32 current_rank = get_rank(user_id);
        for (i32 rank_it = 1; rank_it < m_max_rank; rank_it++)
        {
            if (get_collected_stars(user_id, rank_it) >= get_stars_for_rank(rank_it + 1))
            {
                current_rank = rank_it + 1;
            }
        }
        if (current_rank != get_rank(user_id))
        {
            update_rank(user_id, current_rank);
        }
    }
}
