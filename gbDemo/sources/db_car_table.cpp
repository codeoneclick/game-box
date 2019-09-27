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
                                          "([id] INTEGER NO NULL, [garage_id] INTEGER NO NULL, [is_openned] INTEGER NO NULL, [is_bought] INTEGER NO NULL, [price] INTEGER NO NULL, [car_skin_id] INTEGER NO NULL, [car_body_color_id] INTEGER NO NULL, [car_windows_color_id] INTEGER NO NULL, [car_speed_upgrade] INTEGER NO NULL, [car_handling_upgrade] INTEGER NO NULL, [car_rigidity_upgrade] INTEGER NO NULL, [revision] INTEGER NO NULL, [data] BLOB NOT NULL, constraint [pk_id] primary key ([id]));");
        
        if (!is_column_exist("is_bought"))
        {
            if (!add_new_column("is_bought"))
            {
                assert(false);
            }
        }
        
        if (!is_column_exist("price"))
        {
            if (!add_new_column("price"))
            {
                assert(false);
            }
        }
        
        
        if (!is_column_exist("m_car_main_color_r"))
        {
            if (!add_new_column("m_car_main_color_r"))
            {
                assert(false);
            }
        }
        
        if (!is_column_exist("m_car_main_color_g"))
        {
            if (!add_new_column("m_car_main_color_g"))
            {
                assert(false);
            }
        }
        
        if (!is_column_exist("m_car_main_color_b"))
        {
            if (!add_new_column("m_car_main_color_b"))
            {
                assert(false);
            }
        }
        
        if (!is_column_exist("m_car_second_color_r"))
        {
            if (!add_new_column("m_car_second_color_r"))
            {
                assert(false);
            }
        }
        
        if (!is_column_exist("m_car_second_color_g"))
        {
            if (!add_new_column("m_car_second_color_g"))
            {
                assert(false);
            }
        }
        
        if (!is_column_exist("m_car_second_color_b"))
        {
            if (!add_new_column("m_car_second_color_b"))
            {
                assert(false);
            }
        }
        
        if (!is_column_exist("car_speed_upgrade"))
        {
            if (!add_new_column("car_speed_upgrade"))
            {
                assert(false);
            }
        }
        
        if (!is_column_exist("car_handling_upgrade"))
        {
            if (!add_new_column("car_handling_upgrade"))
            {
                assert(false);
            }
        }
        
        if (!is_column_exist("car_rigidity_upgrade"))
        {
            if (!add_new_column("car_rigidity_upgrade"))
            {
                assert(false);
            }
        }
        
        if (!is_column_exist("revision"))
        {
            if (!add_new_column("revision"))
            {
                assert(false);
            }
        }
        
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
        predicate<<"insert or replace into cars(id, garage_id, is_openned, is_bought, price, car_skin_id, car_body_color_id, car_windows_color_id, m_car_main_color_r, m_car_main_color_g, m_car_main_color_b, m_car_second_color_r, m_car_second_color_g, m_car_second_color_b, car_speed_upgrade, car_handling_upgrade, car_rigidity_upgrade, revision, data) values("<<id<<","<<data.m_garage_id<<","<<data.m_is_openned<<","<<data.m_is_bought<<","<<data.m_price<<","<<-1<<","<<-1<<","<<-1<<","<<data.m_car_main_color_r<<","<<data.m_car_main_color_g<<","<<data.m_car_main_color_b<<","<<data.m_car_second_color_r<<","<<data.m_car_second_color_g<<","<<data.m_car_second_color_b<<","<<data.m_car_speed_upgrade<<","<<data.m_car_handling_upgrade<<","<<data.m_car_rigidity_upgrade<<","<<data.m_revision<<",?);";
        bool result = m_database->insert(predicate.str(), raw_data, size, 1);
        return result;
    }
}
