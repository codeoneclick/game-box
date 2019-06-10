//
//  db_level_table.cpp
//  gbDemo
//
//  Created by serhii.s on 6/10/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "db_level_table.h"
#include "database_connection.h"
#include "database_records_container.h"

namespace game
{
    db_level_table::db_level_table(const gb::db::database_connection_shared_ptr& database) :
    gb::db::database_table(database, "levels")
    {
        
    }
    
    void db_level_table::construct()
    {
        bool result = m_database->execute("CREATE TABLE IF NOT EXISTS [levels]"
                                          "([id] INTEGER NO NULL, [level_index] INTEGER NO NULL, [is_openned] INTEGER NO NULL,  [is_passed] INTEGER NO NULL, [drift_time] INTEGER NO NULL, [stars_count] INTEGER NO NULL, [data] BLOB NOT NULL, constraint [pk_id] primary key ([id]));");
        if (!result)
        {
            assert(false);
        }
    }
    
    bool db_level_table::save_to_db(i32 id, const db_level_data& data)
    {
        const i32 size = sizeof(db_level_data);
        char raw_data[size];
        memcpy(raw_data, &data, size);
        
        std::stringstream predicate;
        predicate<<"insert or replace into levels(id, level_index, is_openned, is_passed, drift_time, stars_count, data) values("<<id<<","<<data.m_level_index<<","<<data.m_is_openned<<","<<data.m_is_passed<<","<<data.m_drift_time<<","<<data.m_stars_count<<",?);";
        bool result = m_database->insert(predicate.str(), raw_data, size, 1);
        return result;
    }
}
