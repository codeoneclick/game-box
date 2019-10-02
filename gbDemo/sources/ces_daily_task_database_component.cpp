//
//  ces_daily_task_database_component.cpp
//  gbDemo
//
//  Created by serhii.s on 9/30/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_daily_task_database_component.h"
#include "db_daily_task_table.h"
#include "database_entity.h"

namespace game
    {
    const i32 ces_daily_task_database_component::k_visit_game_task = 1;
    const i32 ces_daily_task_database_component::k_open_one_level_task = 2;
    const i32 ces_daily_task_database_component::k_paint_car_task = 3;
    const i32 ces_daily_task_database_component::k_upgrade_car_task = 4;
    const i32 ces_daily_task_database_component::k_get_3_first_place_achievement_task = 5;
    const i32 ces_daily_task_database_component::k_get_3_good_drift_achievement_task = 6;
    const i32 ces_daily_task_database_component::k_get_3_no_damage_achievement_task = 7;
    const i32 ces_daily_task_database_component::k_race_5_task = 8;
    const i32 ces_daily_task_database_component::k_drift_5_minutes = 9;
    
    ces_daily_task_database_component::daily_task_dto::daily_task_dto(const gb::db::database_coordinator_shared_ptr& database_coordinator) :
    m_database_coordinator(database_coordinator)
    {
        
    }
    
    i32 ces_daily_task_database_component::daily_task_dto::get_id() const
    {
        return m_id;
    }
    
    i32 ces_daily_task_database_component::daily_task_dto::get_progress() const
    {
        return m_progress;
    }
    
    i32 ces_daily_task_database_component::daily_task_dto::get_reward_id() const
    {
        return m_reward_id;
    }
    
    bool ces_daily_task_database_component::daily_task_dto::get_is_done() const
    {
        return m_is_done;
    }
    
    bool ces_daily_task_database_component::daily_task_dto::get_is_claimed() const
    {
        return m_claimed;
    }
    
    i32 ces_daily_task_database_component::daily_task_dto::get_day() const
    {
        return m_day;
    }
    
    i32 ces_daily_task_database_component::daily_task_dto::get_cash_reward() const
    {
        return m_cash_reward;
    }
    
    i32 ces_daily_task_database_component::daily_task_dto::get_requirements() const
    {
        return m_requirements;
    }
    
    ces_daily_task_database_component::ces_daily_task_database_component()
    {
        
    }
    
    i32 ces_daily_task_database_component::get_current_day()
    {
        time_t tt;
        struct tm * ti;
        time (&tt);
        ti = localtime(&tt);
        return ti->tm_yday;
    }
    
    void ces_daily_task_database_component::set_database_coordinator(const gb::db::database_coordinator_shared_ptr& database_coordinator)
    {
        m_database_coordinator = database_coordinator;
    }
    
    
    void ces_daily_task_database_component::validate_all_tasks()
    {
        for (i32 i = 1; i <= 9; ++i)
        {
            auto daily_task_record = std::make_shared<gb::db::database_entity<db_daily_task_table, db_daily_task_data>>(m_database_coordinator.lock());
            if(!daily_task_record->load_from_db(i))
            {
                auto& data = daily_task_record->get_data();
                data.m_id = i;
                data.m_reward_id = 0;
                data.m_progress = 0;
                data.m_is_done = 0;
                data.m_claimed = 0;
                data.m_day = get_current_day();
                daily_task_record->save_to_db();
            }
            else
            {
                auto& data = daily_task_record->get_data();
                if (get_current_day() > data.m_day)
                {
                    data.m_progress = 0;
                    data.m_is_done = 0;
                    data.m_claimed = 0;
                    data.m_day = get_current_day();
                    daily_task_record->save_to_db();
                }
            }
        }
    }
    
    bool ces_daily_task_database_component::is_some_task_done()
    {
        bool result = false;
        for (i32 index = 1; index <= 9; ++index)
        {
            auto daily_task_record = std::make_shared<gb::db::database_entity<db_daily_task_table, db_daily_task_data>>(m_database_coordinator.lock());
            if(!daily_task_record->load_from_db(index))
            {
                assert(false);
            }
            else
            {
                auto& data = daily_task_record->get_data();
                if (data.m_is_done == 1 && data.m_claimed == 0)
                {
                    result = true;
                    break;
                }
            }
        }
        return result;
    }
    
    std::unordered_map<i32, std::shared_ptr<ces_daily_task_database_component::daily_task_dto>> ces_daily_task_database_component::get_all_tasks()
    {
        m_tasks.clear();
        for (i32 index = 1; index <= 9; ++index)
        {
            auto daily_task_record = std::make_shared<gb::db::database_entity<db_daily_task_table, db_daily_task_data>>(m_database_coordinator.lock());
            if(!daily_task_record->load_from_db(index))
            {
                assert(false);
            }
            else
            {
                auto& data = daily_task_record->get_data();
                if (get_current_day() > data.m_day)
                {
                    data.m_progress = 0;
                    data.m_is_done = 0;
                    data.m_claimed = 0;
                    data.m_day = get_current_day();
                    daily_task_record->save_to_db();
                }
                
                const auto daily_task_dto = std::make_shared<ces_daily_task_database_component::daily_task_dto>(m_database_coordinator.lock());
                daily_task_dto->m_id = data.m_id;
                daily_task_dto->m_reward_id = data.m_reward_id;
                daily_task_dto->m_progress = data.m_progress;
                daily_task_dto->m_is_done = data.m_is_done;
                daily_task_dto->m_claimed = data.m_claimed;
                
                daily_task_dto->m_cash_reward = get_task_cash_reward(index);
                daily_task_dto->m_requirements = get_task_requirements(index);
                m_tasks.insert(std::make_pair(index, daily_task_dto));
            }
        }
        return m_tasks;
    }
    
    std::shared_ptr<ces_daily_task_database_component::daily_task_dto> ces_daily_task_database_component::get_task(i32 task_id)
    {
        std::shared_ptr<ces_daily_task_database_component::daily_task_dto> task = nullptr;
        const auto tasks = get_all_tasks();
        const auto task_it = tasks.find(task_id);
        if (task_it != tasks.end())
        {
            task = task_it->second;
        }
        return task;
    }
    
    void ces_daily_task_database_component::update_task_progress(i32 task_id, i32 progress)
    {
        auto daily_task_record = std::make_shared<gb::db::database_entity<db_daily_task_table, db_daily_task_data>>(m_database_coordinator.lock());
        if(daily_task_record->load_from_db(task_id))
        {
            auto& data = daily_task_record->get_data();
            data.m_progress = progress;
            daily_task_record->save_to_db();
        }
    }
    
    void ces_daily_task_database_component::inc_task_progress(i32 task_id, i32 value)
    {
        auto daily_task_record = std::make_shared<gb::db::database_entity<db_daily_task_table, db_daily_task_data>>(m_database_coordinator.lock());
        if(daily_task_record->load_from_db(task_id))
        {
            auto& data = daily_task_record->get_data();
            data.m_progress += value;
            daily_task_record->save_to_db();
        }
    }
    
    void ces_daily_task_database_component::set_task_is_done(i32 task_id)
    {
        auto daily_task_record = std::make_shared<gb::db::database_entity<db_daily_task_table, db_daily_task_data>>(m_database_coordinator.lock());
        if(daily_task_record->load_from_db(task_id))
        {
            auto& data = daily_task_record->get_data();
            data.m_is_done = 1;
            daily_task_record->save_to_db();
        }
    }
    
    void ces_daily_task_database_component::set_task_is_claimed(i32 task_id)
    {
        auto daily_task_record = std::make_shared<gb::db::database_entity<db_daily_task_table, db_daily_task_data>>(m_database_coordinator.lock());
        if(daily_task_record->load_from_db(task_id))
        {
            auto& data = daily_task_record->get_data();
            data.m_claimed = 1;
            daily_task_record->save_to_db();
        }
    }
    
    i32 ces_daily_task_database_component::get_task_cash_reward(i32 task_id)
    {
        switch (task_id) {
            case k_visit_game_task:
            {
                return 500;
            }
                break;
                
            case k_open_one_level_task:
            {
                return 500;
            }
                break;
                
            case k_paint_car_task:
            {
                return 100;
            }
                break;
                
            case k_upgrade_car_task:
            {
                return 1500;
            }
                break;
                
            case k_get_3_first_place_achievement_task:
            {
                return 1000;
            }
                break;
                
            case k_get_3_good_drift_achievement_task:
            {
                return 1000;
            }
                break;
                
            case k_get_3_no_damage_achievement_task:
            {
                return 1000;
            }
                break;
                
            case k_race_5_task:
            {
                return 750;
            }
                break;
                
            case k_drift_5_minutes:
            {
                return 2000;
            }
                break;
                
            default:
                break;
        }
        
        return 0;
    }
    
    i32 ces_daily_task_database_component::get_task_requirements(i32 task_id)
    {
        switch (task_id) {
            case k_visit_game_task:
            {
                return 1;
            }
                break;
                
            case k_open_one_level_task:
            {
                return 1;
            }
                break;
                
            case k_paint_car_task:
            {
                return 1;
            }
                break;
                
            case k_upgrade_car_task:
            {
                return 1;
            }
                break;
                
            case k_get_3_first_place_achievement_task:
            {
                return 3;
            }
                break;
                
            case k_get_3_good_drift_achievement_task:
            {
                return 3;
            }
                break;
                
            case k_get_3_no_damage_achievement_task:
            {
                return 3;
            }
                break;
                
            case k_race_5_task:
            {
                return 5;
            }
                break;
                
            case k_drift_5_minutes:
            {
                return 5 * 60;
            }
                break;
                
            default:
                break;
        }
        
        return 0;
    }
    
    std::string ces_daily_task_database_component::get_task_name(i32 task_id) const
    {
        switch (task_id) {
            case k_visit_game_task:
            {
                return "VISIT";
            }
                break;
                
            case k_open_one_level_task:
            {
                return "OPEN";
            }
                break;
                
            case k_paint_car_task:
            {
                return "PAINT";
            }
                break;
                
            case k_upgrade_car_task:
            {
                return "UPGRADE";
            }
                break;
                
            case k_get_3_first_place_achievement_task:
            {
                return "FIRST";
            }
                break;
                
            case k_get_3_good_drift_achievement_task:
            {
                return "DRIFT";
            }
                break;
                
            case k_get_3_no_damage_achievement_task:
            {
                return "DAMAGE";
            }
                break;
                
            case k_race_5_task:
            {
                return "RACE";
            }
                break;
                
            case k_drift_5_minutes:
            {
                return "TIME";
            }
                break;
                
            default:
                break;
        }
        return "UNDEFINED";
    }
    
    std::string ces_daily_task_database_component::get_task_description(i32 task_id) const
    {
        switch (task_id) {
            case k_visit_game_task:
            {
                return "VISIT GAME EVERY DAY TO RECEIVE ADDITIONAL CASH";
            }
                break;
                
            case k_open_one_level_task:
            {
                return "OPEN NEW LEVEL IN CAREER MODE";
            }
                break;
                
            case k_paint_car_task:
            {
                return "RE-PAINT YOUR CAR, PEOPLE LOVE YOUR STYLE";
            }
                break;
                
            case k_upgrade_car_task:
            {
                return "UPGRADE PERFORMANCE OF YOUR CAR TO BE FIRST IN ALL CHALLENGES";
            }
                break;
                
            case k_get_3_first_place_achievement_task:
            {
                return "GET -- FIRST PLACE -- ACHIEVEMENT 3 TIMES";
            }
                break;
                
            case k_get_3_good_drift_achievement_task:
            {
                return "GET -- GOOD DRIFT -- ACHIEVEMENT 3 TIMES";
            }
                break;
                
            case k_get_3_no_damage_achievement_task:
            {
                return "GET -- LOW DAMAGE -- ACHIEVEMENT 3 TIMES";
            }
                break;
                
            case k_race_5_task:
            {
                return "JUST RACE 5 TIMES";
            }
                break;
                
            case k_drift_5_minutes:
            {
                return "DRIFT 5 MINUTES OR MORE...";
            }
                break;
                
            default:
                break;
        }
        
        return "UNDEFINED";
    }
}
