//
//  db_daily_task_table.cpp
//  gbDemo
//
//  Created by serhii.s on 10/1/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "db_daily_task_table.h"
#include "database_connection.h"
#include "database_records_container.h"

namespace game
{
    db_daily_task_table::db_daily_task_table(const gb::db::database_connection_shared_ptr& database) :
    gb::db::database_table(database, "daily_task")
    {
        
    }
    
    void db_daily_task_table::construct()
    {
        bool result = m_database->execute("CREATE TABLE IF NOT EXISTS [daily_task]"
                                          "([id] INTEGER NO NULL, [progress] INTEGER NO NULL, [reward_id] INTEGER NO NULL, [is_done] INTEGER NO NULL, [is_claimed] INTEGER NO NULL, [day] INTEGER NO NULL, [data] BLOB NOT NULL, constraint [pk_id] primary key ([id]));");
        if (!result)
        {
            assert(false);
        }
    }
    
    bool db_daily_task_table::save_to_db(i32 id, const db_daily_task_data& data)
    {
        const i32 size = sizeof(db_daily_task_data);
        char raw_data[size];
        memcpy(raw_data, &data, size);
        
        std::stringstream predicate;
        predicate<<"insert or replace into daily_task(id, progress, reward_id, is_done, is_claimed, day, data) values("<<id<<","<<data.m_progress<<","<<data.m_reward_id<<","<<data.m_is_done<<","<<data.m_claimed<<","<<data.m_day<<",?);";
        bool result = m_database->insert(predicate.str(), raw_data, size, 1);
        return result;
    }
}
