//
//  db_character_quests_table.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 8/8/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "db_character_quests_table.h"
#include "database_connection.h"
#include "database_records_container.h"

namespace game
{
    db_character_quests_table::db_character_quests_table(const gb::db::database_connection_shared_ptr& database) :
    gb::db::database_table(database)
    {
        
    }
    
    void db_character_quests_table::construct()
    {
        bool result = m_database->execute("CREATE TABLE IF NOT EXISTS [quests]"
                                          "([id] INTEGER NO NULL, [data] BLOB NOT NULL, constraint [pk_id] primary key ([id]));");
        if (!result)
        {
            assert(false);
        }
    }
    
    bool db_character_quests_table::load_from_db(i32 id, char* raw_data, i32& size, bool all, const read_data_callback_t& callback)
    {
        std::stringstream predicate;
        predicate<<"select * from quests";
        if(!all)
        {
            predicate<<" where id=="<<id;
        }
        gb::db::database_records_container_shared_ptr result;
        m_database->execute(predicate.str(), result);
        
        if (!result || result->get_records_count() == 0)
        {
            return false;
        }
        
        gb::db::database_records_container::record_iterator it(result);
        while (it)
        {
            size = 0;
            const char* raw_data_ptr = it->get_blob("data", size);
            if (!raw_data || size <= 0)
            {
                return false;
            }
            memcpy(raw_data, raw_data_ptr, size);
            if(callback)
            {
                callback(it->get_i32("id"), raw_data, size);
            }
            ++it;
        }
        return true;
    }
    
    bool db_character_quests_table::save_to_db(i32 id, const char* raw_data, i32 size)
    {
        std::stringstream predicate;
        predicate<<"insert or replace into quests(id, data) values("<<id<<", ?);";
        bool result = m_database->insert(predicate.str(), raw_data, size, 1);
        return result;
    }
}
