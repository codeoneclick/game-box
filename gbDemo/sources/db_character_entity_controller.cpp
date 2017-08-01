//
//  db_character_entity_controller.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 8/1/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "db_character_entity_controller.h"
#include "database_connection.h"
#include "db_character_entity.h"
#include "database_records_container.h"

namespace game
{
    db_character_entity_controller::db_character_entity_controller(const gb::db::database_connection_shared_ptr& database) :
    gb::db::database_entity_controller(database)
    {
        
    }
    
    void db_character_entity_controller::construct()
    {
        bool result = m_database->execute("CREATE TABLE IF NOT EXISTS [characters]"
                                          "([id] INTEGER NO NULL, [data] BLOB NOT NULL, constraint [pk_id] primary key ([id]));");
        
        if (!result)
        {
            assert(false);
        }
    }
    
    bool db_character_entity_controller::load_from_db(const db_character_entity_shared_ptr& entity)
    {
        gb::db::database_records_container_shared_ptr result;
        
        m_database->execute("SELECT * FROM characters", result);
        
        if (!result || result->get_records_count() == 0)
        {
            return false;
        }
        
        gb::db::database_records_container::record_iterator it(result);
        while (it)
        {
            i32 id = it->get_i32("id");
            i32 size = 0;
            const char* raw_data = it->get_blob("data", size);
            if (!raw_data || size <= 0)
            {
                return false;
            }
            db_character_entity::data_t* data = (db_character_entity::data_t*)raw_data;
            ++it;
        }
        
        return true;

    }
    
    bool db_character_entity_controller::save_to_db(const db_character_entity_shared_ptr& entity)
    {
        std::stringstream predicate;
        predicate<<"insert or replace into characters(id, data) values("<<entity->m_id<<", ?);";
        i32 size = sizeof(db_character_entity::data_t);
        char data[size];
        memcpy(data, &entity->m_data, size);
        
        bool result = m_database->insert(predicate.str(), data, size, 1);
        return result;
    }
}
