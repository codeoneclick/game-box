//
//  database_records_container.cpp
//  gb_database
//
//  Created by serhii serhiiv on 7/31/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "database_records_container.h"
#include "sqlite3.h"
#include "database_record.h"

namespace gb
{
    namespace db
    {
        #define PREDICATE [&](const database_record_shared_ptr& record) { return record->get_name() == record_name; }
        
        database_records_container::record_iterator::record_iterator(const database_records_container_shared_ptr& records_container) :
        m_records_container(records_container)
        {
            records_container->move_to_first();
        }
        
        database_records_container::database_records_container(sqlite3_stmt*& statement) :
        m_query(statement)
        {
            m_step_status = sqlite3_step(m_query);
            if (m_step_status == SQLITE_ROW)
            {
                for (i32 i = 0; i < sqlite3_column_count(m_query); i++)
                {
                    database_record_shared_ptr record = std::make_shared<database_record>();
                    switch (sqlite3_column_type(m_query, i))
                    {
                        case SQLITE_INTEGER:
                        {
                            record->set_type(gb::db::database_record::e_database_record_type_int);
                        }
                            break;
                            
                        case SQLITE_FLOAT:
                        {
                            record->set_type(gb::db::database_record::e_database_record_type_float);
                        }
                            break;
                            
                        case SQLITE_TEXT:
                        {
                            record->set_type(gb::db::database_record::e_database_record_type_string);
                        }
                            break;
                            
                        case SQLITE_BLOB:
                        {
                            record->set_type(gb::db::database_record::e_database_record_type_blob);
                        }
                            break;
                        case SQLITE_NULL:
                        {
                            record->set_type(gb::db::database_record::e_database_record_type_unknown);
                        }
                            break;
                            
                        default:
                        {
                            assert(false);
                        }
                            break;
                    }
                    record->set_name(sqlite3_column_name(m_query, i));
                    m_records.push_back(record);
                }
                m_records_count++;
            }
            while (sqlite3_step(m_query) == SQLITE_ROW)
            {
                m_records_count++;
            }
            sqlite3_reset(m_query);
        }
        
        database_records_container::~database_records_container()
        {
            
        }
        
        void database_records_container::move_to_first()
        {
            sqlite3_reset(m_query);
            m_step_status = sqlite3_step(m_query);
        }
        
        void database_records_container::move_to_next()
        {
             m_step_status = sqlite3_step(m_query);
        }
        
        bool database_records_container::is_at_end() const
        {
            return m_step_status == SQLITE_DONE;
        }
        
        i32 database_records_container::get_records_count() const
        {
            return m_records_count;
        }
        
        std::string database_records_container::get_string(const std::string& record_name) const
        {
            size_t index = get_index_with_predicate(m_records, PREDICATE);
            if (index == size_t(-1))
            {
                return "";
            }
            else
            {
                return std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_query, static_cast<i32>(index))));
            }
        }
        
        i32 database_records_container::get_i32(const std::string& record_name) const
        {
            size_t index = get_index_with_predicate(m_records, PREDICATE);
            if (index == size_t(-1))
            {
                return 0;
            }
            else
            {
                return static_cast<i32>(sqlite3_column_int(m_query, static_cast<i32>(index)));
            }
        }
        
        f32 database_records_container::get_f32(const std::string& record_name) const
        {
            size_t index = get_index_with_predicate(m_records, PREDICATE);
            if (index == size_t(-1))
            {
                return 0.f;
            }
            else
            {
                return static_cast<f32>(sqlite3_column_double(m_query, static_cast<i32>(index)));
            }
        }
        
        i64 database_records_container::get_i64(const std::string& record_name) const
        {
            size_t index = get_index_with_predicate(m_records, PREDICATE);
            if (index == size_t(-1))
            {
                return 0;
            }
            else
            {
                return static_cast<i64>(sqlite3_column_int64(m_query, static_cast<i32>(index)));
            }
        }
        
        const char* database_records_container::get_blob(const std::string& record_name, int& size) const
        {
            size_t index = get_index_with_predicate(m_records, PREDICATE);
            if (index == size_t(-1))
            {
                return nullptr;
            }
            else
            {
                size = sqlite3_column_bytes(m_query, static_cast<i32>(index));
                if (size <= 0)
                {
                    return nullptr;
                }
                else
                {
                    const char* data = static_cast<const char*>(sqlite3_column_blob(m_query, static_cast<i32>(index)));
                    return data;
                }
            }
        }
        
        std::vector<database_record_shared_ptr> database_records_container::get_records() const
        {
            return m_records;
        }
    }
}
