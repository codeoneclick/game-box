//
//  db_car_table.cpp
//  gbDemo
//
//  Created by serhii.s on 5/16/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "db_car_table.h"
#include "database_connection.h"
#include "database_records_container.h"

namespace game
{
    db_car_table::db_car_table(const gb::db::database_connection_shared_ptr& database) :
    gb::db::database_table(database, "cars")
    {
        
    }
    
    void db_car_table::construct()
    {
        bool result = m_database->execute("CREATE TABLE IF NOT EXISTS [cars]"
                                          "([id] INTEGER NO NULL, [garage_id] INTEGER NO NULL, [is_openned] INTEGER NO NULL,  [car_skin_id] INTEGER NO NULL, [data] BLOB NOT NULL, constraint [pk_id] primary key ([id]));");
        if (!result)
        {
            assert(false);
        }
    }
    
    bool db_car_table::save_to_db(i32 id, const db_car_data& data)
    {
        const i32 size = sizeof(db_car_data);
        char raw_data[size];
        memcpy(raw_data, &data, size);
        
        std::stringstream predicate;
        predicate<<"insert or replace into cars(id, garage_id, is_openned, car_skin_id, data) values("<<id<<","<<data.m_garage_id<<","<<data.m_is_openned<<","<<data.m_car_skin_id<<",?);";
        bool result = m_database->insert(predicate.str(), raw_data, size, 1);
        return result;
    }
}
