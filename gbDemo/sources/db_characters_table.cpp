//
//  db_characters_table.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 8/1/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "db_characters_table.h"
#include "database_connection.h"
#include "database_records_container.h"

namespace game
{
    db_characters_table::db_characters_table(const gb::db::database_connection_shared_ptr& database) :
    gb::db::database_table(database, "characters")
    {
    }
    
    void db_characters_table::construct()
    {
        bool result = m_database->execute("CREATE TABLE IF NOT EXISTS [characters]"
                                          "([id] INTEGER NO NULL, [data] BLOB NOT NULL, constraint [pk_id] primary key ([id]));");
        if (!result)
        {
            assert(false);
        }
    }
    
    bool db_characters_table::save_to_db(i32 id, const db_character_data& data)
    {
        i32 size = sizeof(db_character_data);
        char raw_data[size];
        memcpy(raw_data, &data, size);
        
        std::stringstream predicate;
        predicate<<"insert or replace into characters(id, data) values("<<id<<", ?);";
        bool result = m_database->insert(predicate.str(), raw_data, size, 1);
        return result;
    }
}
