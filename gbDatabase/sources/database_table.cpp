//
//  database_table.cpp
//  gb_database
//
//  Created by serhii serhiiv on 8/2/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "database_table.h"
#include "database_connection.h"

namespace gb
{
    namespace db
    {
        std::set<uintptr_t> database_table::g_guids_container;
        
        database_table::database_table(const database_connection_shared_ptr& database, const std::string& name) :
        m_database(database),
        m_name(name)
        {
            
        }
        
        bool database_table::load_from_db(i32 id, const read_data_callback_t& callback)
        {
            assert(m_name.length() != 0);
            std::stringstream predicate;
            predicate<<"select * from "<<m_name;
            predicate<<" where id=="<<id;
            return database_table::load_from_db_with_custom_predicate(predicate.str(), callback);
        }
        
        bool database_table::load_all_from_db(const read_data_callback_t& callback)
        {
            assert(m_name.length() != 0);
            std::stringstream predicate;
            predicate<<"select * from "<<m_name;
            return database_table::load_from_db_with_custom_predicate(predicate.str(), callback);
        }
        
        bool database_table::load_from_db_with_custom_predicate(const std::string& predicate, const read_data_callback_t& callback)
        {
            return database_table::load_from_db_with_custom_predicate(predicate, [callback](const gb::db::database_records_container::record_iterator& it) {
                i32 size = 0;
                const char* raw_data_ptr = it->get_blob("data", size);
                if (raw_data_ptr && size > 0)
                {
                    char* raw_data = new char[size];
                    memcpy(raw_data, raw_data_ptr, size);
                    if(callback)
                    {
                        callback(it->get_i32("id"), raw_data, size);
                    }
					delete[] raw_data;
                }
            });
        }
        
        bool database_table::load_from_db_with_custom_predicate(const std::string& predicate, const read_record_callback_t& callback)
        {
            gb::db::database_records_container_shared_ptr result;
            m_database->execute(predicate, result);
            
            if (!result || result->get_records_count() == 0)
            {
                return false;
            }
            
            gb::db::database_records_container::record_iterator it(result);
            while (it)
            {
                if(callback)
                {
                    callback(it);
                }
                ++it;
            }
            return true;
        }
        
        bool database_table::delete_from_db(i32 id)
        {
            assert(m_name.length() != 0);
            std::stringstream predicate;
            predicate<<"delete from "<<m_name;
            predicate<<" where id=="<<id;
            return m_database->remove(predicate.str());
        }
    }
}
