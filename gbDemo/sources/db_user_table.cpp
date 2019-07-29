//
//  db_user_table.cpp
//  gbDemo
//
//  Created by serhii.s on 6/14/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "db_user_table.h"
#include "database_connection.h"
#include "database_records_container.h"

namespace game
{
    db_user_table::db_user_table(const gb::db::database_connection_shared_ptr& database) :
    gb::db::database_table(database, "user")
    {
        
    }
    
    void db_user_table::construct()
    {
        bool result = m_database->execute("CREATE TABLE IF NOT EXISTS [user]"
                                          "([id] INTEGER NO NULL, [tickets] INTEGER NO NULL, [cash] INTEGER NO NULL, [last_ticket_dec_timestamp] INTEGER NO NULL, [rank] INTEGER NO NULL, [claimed_rank] INTEGER NO NULL, [stars_collected] INTEGER NO NULL, [ftue_step] INTEGER NO NULL, [revision] INTEGER NO NULL, [data] BLOB NOT NULL, constraint [pk_id] primary key ([id]));");
        if (!is_column_exist("cash"))
        {
            if (!add_new_column("cash"))
            {
                assert(false);
            }
        }
        
        if (!is_column_exist("ftue_step"))
        {
            if (!add_new_column("ftue_step"))
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
    
    bool db_user_table::save_to_db(i32 id, const db_user_data& data)
    {
        const i32 size = sizeof(db_user_data);
        char raw_data[size];
        memcpy(raw_data, &data, size);
        
        std::stringstream predicate;
        predicate<<"insert or replace into user(id, tickets, cash, last_ticket_dec_timestamp, rank, claimed_rank, stars_collected, ftue_step, revision, data) values("<<id<<","<<data.m_tickets<<","<<data.m_cash<<","<<data.m_last_ticket_dec_timestamp<<","<<data.m_rank<<","<<data.m_claimed_rank<<","<<data.m_stars_collected<<","<<data.m_ftue_step<<","<<data.m_revision<<", ?);";
        bool result = m_database->insert(predicate.str(), raw_data, size, 1);
        return result;
    }
}
