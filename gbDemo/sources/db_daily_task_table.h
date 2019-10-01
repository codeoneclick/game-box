//
//  db_daily_task_table.hpp
//  gbDemo
//
//  Created by serhii.s on 10/1/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "db_declarations.h"
#include "ns_declarations.h"
#include "database_table.h"

namespace game
{
    struct db_daily_task_data
    {
        i32 m_id;
        i32 m_progress;
        i32 m_reward_id;
        i32 m_is_done;
        i32 m_claimed;
        i32 m_day;
    };
    
    class db_daily_task_table : public gb::db::database_table
    {
    private:
        
    protected:
        
    public:
        
        STTI_CLASS_GUID(db_daily_task_table, gb::db::database_table::g_guids_container, override)
        db_daily_task_table(const gb::db::database_connection_shared_ptr& database);
        
        void construct() override;
        bool save_to_db(i32 id, const db_daily_task_data& data);
    };
};


