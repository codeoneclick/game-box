//
//  database_connection.h
//  gb_database
//
//  Created by serhii serhiiv on 7/31/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "db_declarations.h"

struct sqlite3;
struct sqlite3_stmt;

namespace gb
{
    namespace db
    {
        class database_connection
        {
        private:
            
            sqlite3* m_db = nullptr;
            
        protected:
            
        public:
            
            database_connection();
            ~database_connection();

            bool open(const std::string& connection);
            void close();
            
            bool execute(const std::string& command);
            bool execute(const std::string& command, database_records_container_shared_ptr& records, sqlite3_stmt** statement);
            
            bool insert(const std::string& command, const char* data, i32 size, i32 index, const std::string& operation_name = "");
            bool remove(const std::string& command);
        };
    };
};

