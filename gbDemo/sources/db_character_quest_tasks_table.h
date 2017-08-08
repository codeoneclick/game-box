//
//  db_character_quest_tasks_table.h
//  gbDemo
//
//  Created by serhii serhiiv on 8/8/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "db_declarations.h"
#include "ns_declarations.h"
#include "database_table.h"

namespace game
{
    struct db_character_quest_task_entity
    {
        i32 m_id;
        i32 m_quest_id;
        i32 m_type;
        i32 m_goal_id;
        i32 m_max_goal_count;
        i32 m_current_goal_count;
        f32 m_max_goal_time;
        f32 m_current_goal_time;
    };
    
    class db_character_quest_tasks_table : public gb::db::database_table
    {
    private:
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(db_character_quest_tasks_table, gb::db::database_table::g_guids_container, override)
        db_character_quest_tasks_table(const gb::db::database_connection_shared_ptr& database);
        
        void construct() override;
        
        bool load_from_db(i32 id, char* raw_data, i32& size, bool all = false, const read_data_callback_t& callback = nullptr) override;
        bool save_to_db(i32 id, const char* raw_data, i32 size) override;
    };
};

