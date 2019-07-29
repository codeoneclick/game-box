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
        
        bool database_connection::execute(const std::string& command, database_records_container_shared_ptr& records, sqlite3_stmt** statement)
        {
            i32 status = sqlite3_prepare_v2(m_db, command.c_str(), -1, statement, nullptr);
            bool retval = false;
            if (status == SQLITE_OK)
            {
                records.reset(new database_records_container((*statement)));
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
        
        bool database_connection::remove(const std::string& command)
        {
            return database_connection::execute(command);
        }
        
        bool database_connection::is_column_exist(const std::string& table_name, const std::string& column_name)
        {
            bool result = false;
            i32 status = sqlite3_table_column_metadata(m_db, NULL, table_name.c_str(), column_name.c_str(), 0, 0, 0, 0, 0);
            result = status == SQLITE_OK;
            return result;
        }
        
        bool database_connection::add_new_column(const std::string& table_name, const std::string& column_name)
        {
            std::stringstream command_stream;
            command_stream<<"ALTER TABLE [";
            command_stream<<table_name<<"] ";
            command_stream<<"ADD COLUMN [";
            command_stream<<column_name<<"] INTEGER NO NULL";
            
            std::string command = command_stream.str();
            bool retval = execute(command);
            return retval;
        }
    }
}
