//
//  database_connection.cpp
//  gb_database
//
//  Created by serhii serhiiv on 7/31/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "database_connection.h"
#include "sqlite3.h"
#include "database_record.h"
#include "database_records_container.h"

namespace gb
{
    namespace db
    {
        
        database_connection::database_connection()
        {
            
        }
        
        database_connection::~database_connection()
        {
            
        }
        
        bool database_connection::open(const std::string& connection)
        {
            if (m_db)
            {
                return false;
            }
            i32 result = sqlite3_open(connection.c_str(), &m_db);
            return result == SQLITE_OK;
        }
        
        void database_connection::close()
        {
            if (m_db)
            {
                sqlite3_close(m_db);
            }
        }
        
        bool database_connection::execute(const std::string& command)
        {
            sqlite3_stmt* statement = nullptr;
            i32 global_status = sqlite3_prepare_v2(m_db, command.c_str(), -1, &statement, nullptr);
            bool retval = false;
            if (global_status == SQLITE_OK)
            {
                i32 step_status = SQLITE_ERROR;
                do
                {
                    step_status = sqlite3_step(statement);
                    
                } while ((step_status != SQLITE_ERROR) && (step_status != SQLITE_CONSTRAINT) && (step_status != SQLITE_DONE));
                if (step_status == SQLITE_DONE)
                {
                    retval = true;
                }
                else
                {
                    retval = false;
                }
            }
            else
            {
                retval = false;
            }
            sqlite3_finalize(statement);
            return retval;
        }
        
        bool database_connection::execute(const std::string& command, database_records_container_shared_ptr& records)
        {
            sqlite3_stmt* statement;
            i32 global_status = sqlite3_prepare_v2(m_db, command.c_str(), -1, &statement, nullptr);
            bool retval = false;
            if (global_status == SQLITE_OK)
            {
                records.reset(new database_records_container(statement));
                retval = true;
            }
            else
            {
                records.reset();
                retval = false;
            }
            return retval;
        }
        
        bool database_connection::insert(const std::string& command, const char* data, i32 size, i32 index, const std::string& operation_name)
        {
            bool retval = true;
            sqlite3_stmt* statement = nullptr;
            i32 status  = sqlite3_prepare_v2(m_db, command.c_str(), -1, &statement, nullptr);
            if (status != SQLITE_OK)
            {
                const char* dbg = sqlite3_errmsg(m_db);
                std::cout<<dbg<<std::endl;
                retval = false;
            }
            else
            {
                status = sqlite3_bind_blob(statement, index, data, size, SQLITE_STATIC);
                if (status != SQLITE_OK)
                {
                    retval = false;
                }
                else
                {
                    status = sqlite3_step(statement);
                    if (status != SQLITE_DONE)
                    {
                        retval = false;
                    }
                }
            }
            status = sqlite3_finalize(statement);
            return retval;
        }
    }
}
