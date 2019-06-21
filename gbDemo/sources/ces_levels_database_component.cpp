//
//  ces_levels_database_component.cpp
//  gbDemo
//
//  Created by serhii.s on 6/10/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_levels_database_component.h"
#include "db_level_table.h"
#include "database_entity.h"

namespace game
{
    ces_levels_database_component::level_dto::level_dto(const gb::db::database_coordinator_shared_ptr& database_coordinator) :
    m_database_coordinator(database_coordinator)
    {
        
    }
    
    i32 ces_levels_database_component::level_dto::get_id() const
    {
        return m_id;
    }
    
    i32 ces_levels_database_component::level_dto::get_level_index() const
    {
        return m_level_index;
    }
    
    bool ces_levels_database_component::level_dto::get_is_openned() const
    {
        return m_is_openned;
    }
    
    bool ces_levels_database_component::level_dto::get_is_passed() const
    {
        return m_is_passed;
    }
    
    i32 ces_levels_database_component::level_dto::get_drift_time() const
    {
        return m_drift_time;
    }
    
    bool ces_levels_database_component::level_dto::get_is_star_1_received() const
    {
        return m_star_1_received;
    }
    
    bool ces_levels_database_component::level_dto::get_is_star_2_received() const
    {
        return m_star_2_received;
    }
    
    bool ces_levels_database_component::level_dto::get_is_star_3_received() const
    {
        return m_star_3_received;
    }
    
    std::string ces_levels_database_component::level_dto::get_scene_filename() const
    {
        return m_scene_filename;
    }
    
    i32 ces_levels_database_component::level_dto::get_required_drift_time() const
    {
        return m_required_drift_time;
    }
    
    i32 ces_levels_database_component::level_dto::get_session_time_in_seconds() const
    {
        return m_session_time_in_seconds;
    }
    
    i32 ces_levels_database_component::level_dto::get_ai_cars_count() const
    {
        return m_ai_cars_count;
    }
    
    f32 ces_levels_database_component::level_dto::get_complexity() const
    {
        return m_complexity;
    }
    
    ces_levels_database_component::ces_levels_database_component()
    {
        
    }
    
    void ces_levels_database_component::set_database_coordinator(const gb::db::database_coordinator_shared_ptr& database_coordinator)
    {
        m_database_coordinator = database_coordinator;
    }
    
    bool ces_levels_database_component::is_level_exist(i32 level_id) const
    {
        auto level_record = std::make_shared<gb::db::database_entity<db_level_table, db_level_data>>(m_database_coordinator.lock());
        return level_record->load_from_db(level_id);
    }
    
    void ces_levels_database_component::add_level(i32 level_id, const std::shared_ptr<gb::level_configuration>& level_configuration)
    {
        auto level_record = std::make_shared<gb::db::database_entity<db_level_table, db_level_data>>(m_database_coordinator.lock());
        if(!level_record->load_from_db(level_id))
        {
            auto& data = level_record->get_data();
            data.m_id = level_id;
            data.m_level_index = level_id;
            data.m_is_openned = false;
            data.m_is_passed = false;
            data.m_drift_time = 0;
            data.m_star_1_received = false;
            data.m_star_2_received = false;
            data.m_star_3_received = false;
            level_record->save_to_db();
        }
        m_levels_configurations.insert(std::make_pair(level_id, level_configuration));
    }
    
    std::unordered_map<i32, std::shared_ptr<ces_levels_database_component::level_dto>> ces_levels_database_component::get_all_levels()
    {
        m_levels.clear();
        for (i32 index = 1; index <= m_levels_configurations.size(); ++index)
        {
            auto level_record = std::make_shared<gb::db::database_entity<db_level_table, db_level_data>>(m_database_coordinator.lock());
            const auto configuration_it = m_levels_configurations.find(index);
            if(!level_record->load_from_db(index) || configuration_it == m_levels_configurations.end())
            {
                assert(false);
            }
            else
            {
                const auto configuration = configuration_it->second;
                auto& data = level_record->get_data();
                
                const auto level_dto = std::make_shared<ces_levels_database_component::level_dto>(m_database_coordinator.lock());
                level_dto->m_id = data.m_id;
                level_dto->m_level_index = data.m_level_index;
                level_dto->m_is_openned = data.m_is_openned;
                level_dto->m_is_passed = data.m_is_passed;
                level_dto->m_drift_time = data.m_drift_time;
                level_dto->m_star_1_received = data.m_star_1_received != 0;
                level_dto->m_star_2_received = data.m_star_2_received != 0;
                level_dto->m_star_3_received = data.m_star_3_received != 0;
                level_dto->m_scene_filename = configuration->get_scene_filename();
                level_dto->m_required_drift_time = configuration->get_required_drift_time();
                level_dto->m_complexity = configuration->get_complexity();
                level_dto->m_session_time_in_seconds = configuration->get_session_time_in_seconds();
                level_dto->m_ai_cars_count = configuration->get_cars_count();
                m_levels.insert(std::make_pair(index, level_dto));
            }
        }
        return m_levels;
    }
    
    std::shared_ptr<ces_levels_database_component::level_dto> ces_levels_database_component::get_level(i32 level_id)
    {
        std::shared_ptr<ces_levels_database_component::level_dto> level = nullptr;
        const auto levels = get_all_levels();
        const auto level_it = levels.find(level_id);
        if (level_it != levels.end())
        {
            level = level_it->second;
        }
        return level;
    }
    
    void ces_levels_database_component::open_level(i32 level_id)
    {
        auto level_record = std::make_shared<gb::db::database_entity<db_level_table, db_level_data>>(m_database_coordinator.lock());
        if(level_record->load_from_db(level_id))
        {
            auto& data = level_record->get_data();
            data.m_is_openned = true;
            level_record->save_to_db();
        }
    }
    
    void ces_levels_database_component::pass_level(i32 level_id)
    {
        auto level_record = std::make_shared<gb::db::database_entity<db_level_table, db_level_data>>(m_database_coordinator.lock());
        if(level_record->load_from_db(level_id))
        {
            auto& data = level_record->get_data();
            data.m_is_openned = true;
            data.m_is_passed = true;
            level_record->save_to_db();
        }
    }
    
    bool ces_levels_database_component::get_is_star_received(i32 level_id, i32 star_index)
    {
        bool result = false;
        const auto level = get_level(level_id);
        if (star_index == 0)
        {
            result = level->get_is_star_1_received();
        }
        else if (star_index == 1)
        {
            result = level->get_is_star_2_received();
        }
        else if (star_index == 2)
        {
            result = level->get_is_star_3_received();
        }
        return result;
    }
    
    void ces_levels_database_component::set_star_received(i32 level_id, i32 star_index)
    {
        auto level_record = std::make_shared<gb::db::database_entity<db_level_table, db_level_data>>(m_database_coordinator.lock());
        if(level_record->load_from_db(level_id))
        {
            auto& data = level_record->get_data();
            if (star_index == 0)
            {
                data.m_star_1_received = 1;
            }
            else if (star_index == 1)
            {
                data.m_star_2_received = 1;
            }
            else if (star_index == 2)
            {
                data.m_star_3_received = 1;
            }
            level_record->save_to_db();
        }
    }
    
    f32 ces_levels_database_component::get_drift_time(i32 level_id)
    {
        const auto level = get_level(level_id);
        return level->get_drift_time();
    }
    
    void ces_levels_database_component::set_drift_time(i32 level_id, f32 drift_time)
    {
        auto level_record = std::make_shared<gb::db::database_entity<db_level_table, db_level_data>>(m_database_coordinator.lock());
        if(level_record->load_from_db(level_id))
        {
            auto& data = level_record->get_data();
            data.m_drift_time = drift_time;
            level_record->save_to_db();
        }
    }
    
    bool ces_levels_database_component::is_level_oppened(i32 level_id)
    {
        const auto level = get_level(level_id);
        return level->get_is_openned();
    }
    
    bool ces_levels_database_component::is_level_passed(i32 level_id)
    {
        const auto level = get_level(level_id);
        return level->get_is_passed();
    }
    
    void ces_levels_database_component::set_playing_level_id(i32 level_id)
    {
        m_playing_level_id = level_id;
    }
    
    i32 ces_levels_database_component::get_playing_level_id()
    {
        return m_playing_level_id;
    }
    
    i32 ces_levels_database_component::get_next_level_id()
    {
        i32 result = 1;
        const auto levels = get_all_levels();
        for (i32 level_id = 1; level_id <= levels.size(); ++level_id)
        {
            if (levels.at(level_id)->get_is_openned() && !levels.at(level_id)->get_is_passed())
            {
                result = level_id;
                break;
            }
        }
        return result;
    }
}
