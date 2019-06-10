//
//  db_garage_table.cpp
//  gbDemo
//
//  Created by serhii.s on 5/16/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "db_garage_table.h"
#include "database_connection.h"
#include "database_records_container.h"

namespace game
{
    db_garage_table::db_garage_table(const gb::db::database_connection_shared_ptr& database) :
    gb::db::database_table(database, "garage")
    {
        
    }
    
    void db_garage_table::construct()
    {
        bool result = m_database->execute("CREATE TABLE IF NOT EXISTS [garage]"
                                          "([id] INTEGER NO NULL, [selected_car_id] INTEGER NO NULL, [data] BLOB NOT NULL, constraint [pk_id] primary key ([id]));");
        if (!result)
        {
            assert(false);
        }
    }
    
    bool db_garage_table::save_to_db(i32 id, const db_garage_data& data)
    {
        const i32 size = sizeof(db_garage_data);
        char raw_data[size];
        memcpy(raw_data, &data, size);
        
        std::stringstream predicate;
        predicate<<"insert or replace into garage(id, selected_car_id, data) values("<<id<<","<<data.m_selected_car_id<<", ?);";
        bool result = m_database->insert(predicate.str(), raw_data, size, 1);
        return result;
    }
}
