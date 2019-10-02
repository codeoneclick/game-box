//
//  ces_daily_task_database_component.h
//  gbDemo
//
//  Created by serhii.s on 9/30/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"
#include "db_declarations.h"
#include "level_configuration.h"

namespace game
{
    class ces_daily_task_database_component : public gb::ces_base_component
    {
    public:
        
        static const i32 k_visit_game_task;
        static const i32 k_open_one_level_task;
        static const i32 k_paint_car_task;
        static const i32 k_upgrade_car_task;
        static const i32 k_get_3_first_place_achievement_task;
        static const i32 k_get_3_good_drift_achievement_task;
        static const i32 k_get_3_no_damage_achievement_task;
        static const i32 k_race_5_task;
        static const i32 k_drift_5_minutes;
        
        class daily_task_dto
        {
        private:
            
            friend class ces_daily_task_database_component;
            
            gb::db::database_coordinator_weak_ptr m_database_coordinator;
            
            i32 m_id;
            i32 m_progress = 0;
            i32 m_reward_id = -1;
            bool m_is_done = false;
            bool m_claimed = false;
            i32 m_day = 0;
            
            i32 m_requirements = -1;
            i32 m_cash_reward = 0;
            
        protected:
            
        public:
            
            daily_task_dto(const gb::db::database_coordinator_shared_ptr& database_coordinator);
            ~daily_task_dto() = default;
            
            i32 get_id() const;
            i32 get_progress() const;
            i32 get_reward_id() const;
            bool get_is_done() const;
            bool get_is_claimed() const;
            i32 get_day() const;
            
            i32 get_requirements() const;
            i32 get_cash_reward() const;
        };
        
    private:
        
        gb::db::database_coordinator_weak_ptr m_database_coordinator;
        std::unordered_map<i32, std::shared_ptr<daily_task_dto>> m_tasks;
        i32 get_current_day();
        
        i32 get_task_cash_reward(i32 task_id);
        i32 get_task_requirements(i32 task_id);
        
    protected:
        
    public:
        
        STTI_CLASS_GUID(ces_daily_task_database_component, gb::ces_base_component::g_guids_container)
        ces_daily_task_database_component();
        ~ces_daily_task_database_component() = default;
        
        void set_database_coordinator(const gb::db::database_coordinator_shared_ptr& database_coordinator);
        
        std::unordered_map<i32, std::shared_ptr<daily_task_dto>> get_all_tasks();
        std::shared_ptr<daily_task_dto> get_task(i32 task_id);
        
        std::string get_task_name(i32 task_id) const;
        std::string get_task_description(i32 task_id) const;
        
        void update_task_progress(i32 task_id, i32 progress);
        void inc_task_progress(i32 task_id, i32 value);
        void set_task_is_done(i32 task_id);
        void set_task_is_claimed(i32 task_id);
        
        void validate_all_tasks();
        
        bool is_some_task_done();
    };
};


