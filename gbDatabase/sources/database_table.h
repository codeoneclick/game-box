//
//  database_table.h
//  gb_database
//
//  Created by serhii serhiiv on 8/2/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "db_declarations.h"

namespace gb
{
    namespace db
    {
        class database_table
        {
        public:
            
            typedef std::function<void(i32, char*, i32)> read_data_callback_t;
            
        private:
            
        protected:
            
            database_connection_shared_ptr m_database;
            static std::set<uintptr_t> g_guids_container;
            
        public:
            
            CTTI_CLASS_GUID(database_table, database_table::g_guids_container)
            database_table(const database_connection_shared_ptr& database);
            
            virtual void construct() = 0;
            
            virtual bool load_from_db(i32 id, char* raw_data, i32& size, bool all = false, const read_data_callback_t& callback = nullptr) = 0;
            virtual bool save_to_db(i32 id, const char* raw_data, i32 size) = 0;
        };
    };
};
