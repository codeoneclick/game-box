//
//  ces_garage_database_component.cpp
//  gbDemo
//
//  Created by serhii.s on 5/16/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_garage_database_component.h"
#include "db_garage_table.h"
#include "db_car_table.h"
#include "database_entity.h"

namespace game
{
    ces_garage_database_component::garage_dto::car_dto::car_dto(const gb::db::database_coordinator_shared_ptr& database_coordinator) :
    m_database_coordinator(database_coordinator)
    {
        
    }
    
    i32 ces_garage_database_component::garage_dto::car_dto::get_id() const
    {
        return m_id;
    }
    
    i32 ces_garage_database_component::garage_dto::car_dto::get_skin_id() const
    {
        return m_car_skin_id;
    }
    
    ces_garage_database_component::garage_dto::garage_dto(const gb::db::database_coordinator_shared_ptr& database_coordinator) :
    m_database_coordinator(database_coordinator)
    {
        
    }
    
    ces_garage_database_component::ces_garage_database_component()
    {
        
    }
    
    void ces_garage_database_component::set_database_coordinator(const gb::db::database_coordinator_shared_ptr& database_coordinator)
    {
        m_database_coordinator = database_coordinator;
    }
    
    bool ces_garage_database_component::is_garage_exist(i32 garage_id) const
    {
        auto garage_record = std::make_shared<gb::db::database_entity<db_garage_table, db_garage_data>>(m_database_coordinator.lock());
        return garage_record->load_from_db(garage_id);
    }
    
    void ces_garage_database_component::add_garage(i32 garage_id)
    {
        auto garage_record = std::make_shared<gb::db::database_entity<db_garage_table, db_garage_data>>(m_database_coordinator.lock());
        if(!garage_record->load_from_db(garage_id))
        {
            auto& data = garage_record->get_data();
            data.m_id = garage_id;
            garage_record->save_to_db();
        }
    }
    
    bool ces_garage_database_component::is_car_exist(i32 garage_id, i32 car_id) const
    {
        auto garager_record = std::make_shared<gb::db::database_entity<db_garage_table, db_garage_data>>(m_database_coordinator.lock());
        if(!garager_record->load_from_db(garage_id))
        {
            assert(false);
        }
        else
        {
            auto car_record = std::make_shared<gb::db::database_entity<db_car_table, db_car_data>>(m_database_coordinator.lock());
            if(car_record->load_from_db(car_id))
            {
                auto& data = car_record->get_data();
                return data.m_garage_id == garage_id;
            }
        }
        
        return false;
    }
    
    void ces_garage_database_component::add_car_to_garage(i32 garage_id, i32 car_id)
    {
        auto garager_record = std::make_shared<gb::db::database_entity<db_garage_table, db_garage_data>>(m_database_coordinator.lock());
        if(!garager_record->load_from_db(garage_id))
        {
            assert(false);
        }
        else
        {
            auto car_record = std::make_shared<gb::db::database_entity<db_car_table, db_car_data>>(m_database_coordinator.lock());
            if(!car_record->load_from_db(car_id))
            {
                auto& data = car_record->get_data();
                data.m_id = car_id;
                data.m_garage_id = garage_id;
                data.m_is_openned = 0;
                data.m_car_skin_id = 1;
                car_record->save_to_db();
            }
        }
    }
    
    void ces_garage_database_component::select_car(i32 garage_id, i32 car_id)
    {
        auto garager_record = std::make_shared<gb::db::database_entity<db_garage_table, db_garage_data>>(m_database_coordinator.lock());
        if(!garager_record->load_from_db(garage_id))
        {
            assert(false);
        }
        else
        {
            auto& data = garager_record->get_data();
            data.m_selected_car_id = car_id;
            garager_record->save_to_db();
        }
    }
    
    void ces_garage_database_component::select_car_skin(i32 garage_id, i32 car_id, i32 car_skin_id)
    {
        auto garager_record = std::make_shared<gb::db::database_entity<db_garage_table, db_garage_data>>(m_database_coordinator.lock());
        if(!garager_record->load_from_db(garage_id))
        {
            assert(false);
        }
        else
        {
            auto car_record = std::make_shared<gb::db::database_entity<db_car_table, db_car_data>>(m_database_coordinator.lock());
            if(!car_record->load_from_db(car_id))
            {
                assert(false);
            }
            else
            {
                auto& data = car_record->get_data();
                data.m_car_skin_id = car_skin_id;
                car_record->save_to_db();
            }
        }
    }
    
    std::shared_ptr<ces_garage_database_component::garage_dto::car_dto> ces_garage_database_component::get_selected_car(i32 garage_id)
    {
        std::shared_ptr<ces_garage_database_component::garage_dto::car_dto> result = nullptr;
        auto garager_record = std::make_shared<gb::db::database_entity<db_garage_table, db_garage_data>>(m_database_coordinator.lock());
        if(!garager_record->load_from_db(garage_id))
        {
            assert(false);
        }
        else
        {
            auto& data = garager_record->get_data();
            i32 selected_car_id = data.m_selected_car_id;
            auto car_record = std::make_shared<gb::db::database_entity<db_car_table, db_car_data>>(m_database_coordinator.lock());
            if(!car_record->load_from_db(selected_car_id))
            {
                assert(false);
            }
            else
            {
                auto car_dto = std::make_shared<ces_garage_database_component::garage_dto::car_dto>(m_database_coordinator.lock());
                car_dto->m_id = car_record->get_data().m_id;
                car_dto->m_garage_id = car_record->get_data().m_garage_id;
                car_dto->m_is_openned = car_record->get_data().m_is_openned != 0;
                car_dto->m_car_skin_id = car_record->get_data().m_car_skin_id;
                result = car_dto;
            }
        }
        return result;
    }
    
    void ces_garage_database_component::open_car(i32 garage_id, i32 car_id)
    {
        auto garager_record = std::make_shared<gb::db::database_entity<db_garage_table, db_garage_data>>(m_database_coordinator.lock());
        if(!garager_record->load_from_db(garage_id))
        {
            assert(false);
        }
        else
        {
            auto car_record = std::make_shared<gb::db::database_entity<db_car_table, db_car_data>>(m_database_coordinator.lock());
            if(!car_record->load_from_db(car_id))
            {
                assert(false);
            }
            else
            {
                auto& data = car_record->get_data();
                data.m_is_openned = 1;
                car_record->save_to_db();
            }
        }
    }
    
    void ces_garage_database_component::close_car(i32 garage_id, i32 car_id)
    {
        auto garager_record = std::make_shared<gb::db::database_entity<db_garage_table, db_garage_data>>(m_database_coordinator.lock());
        if(!garager_record->load_from_db(garage_id))
        {
            assert(false);
        }
        else
        {
            auto car_record = std::make_shared<gb::db::database_entity<db_car_table, db_car_data>>(m_database_coordinator.lock());
            if(!car_record->load_from_db(car_id))
            {
                assert(false);
            }
            else
            {
                auto& data = car_record->get_data();
                data.m_is_openned = 0;
                car_record->save_to_db();
            }
        }
    }
    
    bool ces_garage_database_component::is_car_oppenned(i32 garage_id, i32 car_id) const
    {
        bool result = false;
        auto garager_record = std::make_shared<gb::db::database_entity<db_garage_table, db_garage_data>>(m_database_coordinator.lock());
        if(!garager_record->load_from_db(garage_id))
        {
            assert(false);
        }
        else
        {
            auto car_record = std::make_shared<gb::db::database_entity<db_car_table, db_car_data>>(m_database_coordinator.lock());
            if(!car_record->load_from_db(car_id))
            {
                assert(false);
            }
            else
            {
                result = car_record->get_data().m_is_openned != 0;
            }
        }
        return result;
    }
    
    i32 ces_garage_database_component::get_previewed_car_id() const
    {
        return m_previewed_car_id;
    }
    
    void ces_garage_database_component::set_previewed_car_id(i32 id)
    {
        m_previewed_car_id = id;
    }
    
    i32 ces_garage_database_component::get_max_cars_count() const
    {
        return m_max_cars_count;
    }
}
